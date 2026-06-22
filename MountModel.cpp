#include "MountModel.hpp"
#include "CoordinateUtils.hpp"
#include <QDebug>
#include <cmath>
#include <algorithm>

static constexpr double DEG2RAD = M_PI / 180.0;
static constexpr double RAD2DEG = 180.0 / M_PI;
static constexpr double ARCSEC_PER_DEG = 3600.0;

MountModel::MountModel(QObject *parent)
    : QObject(parent)
{
}

// ---------------------------------------------------------------------------
// Vector ↔ Direction conversions
// ---------------------------------------------------------------------------

void MountModel::dirToVec(double raDeg, double decDeg,
                           double &vx, double &vy, double &vz)
{
    double ra = raDeg * DEG2RAD;
    double dec = decDeg * DEG2RAD;
    vx = std::cos(dec) * std::cos(ra);
    vy = std::cos(dec) * std::sin(ra);
    vz = std::sin(dec);
}

void MountModel::vecToDir(double vx, double vy, double vz,
                           double &raDeg, double &decDeg)
{
    double r = std::sqrt(vx*vx + vy*vy + vz*vz);
    if (r < 1e-12) { raDeg = 0; decDeg = 0; return; }
    decDeg = std::asin(vz / r) * RAD2DEG;
    raDeg = std::atan2(vy, vx) * RAD2DEG;
    if (raDeg < 0) raDeg += 360.0;
}

void MountModel::altAzToVec(double altDeg, double azDeg,
                             double &vx, double &vy, double &vz)
{
    double alt = altDeg * DEG2RAD;
    double az = azDeg * DEG2RAD;
    // Convention: x=North, y=East, z=Up
    vx = std::cos(alt) * std::cos(az);
    vy = std::cos(alt) * std::sin(az);
    vz = std::sin(alt);
}

void MountModel::vecToAltAz(double vx, double vy, double vz,
                             double &altDeg, double &azDeg)
{
    double r = std::sqrt(vx*vx + vy*vy + vz*vz);
    if (r < 1e-12) { altDeg = 0; azDeg = 0; return; }
    altDeg = std::asin(vz / r) * RAD2DEG;
    azDeg = std::atan2(vy, vx) * RAD2DEG;
    if (azDeg < 0) azDeg += 360.0;
}

// ---------------------------------------------------------------------------
// Observation management
// ---------------------------------------------------------------------------

void MountModel::addObservation(const PointingObservation &obs)
{
    PointingObservation o = obs;

    // Compute error in arcseconds
    double cosDec = std::cos(obs.commandedDecDeg * DEG2RAD);
    o.errorRaArcsec = (obs.actualRaDeg - obs.commandedRaDeg) * cosDec * ARCSEC_PER_DEG;
    o.errorDecArcsec = (obs.actualDecDeg - obs.commandedDecDeg) * ARCSEC_PER_DEG;

    m_observations.append(o);

    emit logMessage(QString("Observation #%1: dRA=%2\" dDec=%3\" at Alt=%4 Az=%5")
                    .arg(m_observations.size())
                    .arg(o.errorRaArcsec, 0, 'f', 1)
                    .arg(o.errorDecArcsec, 0, 'f', 1)
                    .arg(o.altDeg, 0, 'f', 1)
                    .arg(o.azDeg, 0, 'f', 1));

    // Auto-fit if we have enough observations
    if (m_observations.size() >= 4) {
        fitModel();
    }
    if (m_observations.size() >= 3) {
        computeLevelingFromObservations();
    }
}

void MountModel::clear()
{
    m_observations.clear();
    m_coeffs = ModelCoefficients();
    m_modelValid = false;
    m_levelingQ = Quaternion();
    emit modelUpdated(m_coeffs);
    emit levelingUpdated(m_levelingQ);
}

// ---------------------------------------------------------------------------
// TPOINT Model: predicted error in alt/az
//
// For an alt-az mount:
//   dAz  = IH + CH/cos(alt) + NP*tan(alt) + MA*cos(az)*tan(alt) + ME*sin(az)*tan(alt)
//   dAlt = ID + TF*cos(alt) - MA*sin(az) + ME*cos(az) + FO*sin(alt)
// ---------------------------------------------------------------------------

void MountModel::predictModelError(double altDeg, double azDeg,
                                    double &dAzArcsec, double &dAltArcsec) const
{
    double alt = altDeg * DEG2RAD;
    double az = azDeg * DEG2RAD;

    double cosAlt = std::cos(alt);
    double sinAlt = std::sin(alt);
    double tanAlt = (std::abs(cosAlt) > 1e-6) ? sinAlt / cosAlt : 0;
    double secAlt = (std::abs(cosAlt) > 1e-6) ? 1.0 / cosAlt : 0;
    double cosAz = std::cos(az);
    double sinAz = std::sin(az);

    dAzArcsec = m_coeffs.IH
              + m_coeffs.CH * secAlt
              + m_coeffs.NP * tanAlt
              + m_coeffs.MA * cosAz * tanAlt
              + m_coeffs.ME * sinAz * tanAlt;

    dAltArcsec = m_coeffs.ID
               + m_coeffs.TF * cosAlt
               - m_coeffs.MA * sinAz
               + m_coeffs.ME * cosAz
               + m_coeffs.FO * sinAlt;
}

// ---------------------------------------------------------------------------
// Fit model via linear least-squares
//
// Each observation contributes two equations (dAz, dAlt).
// The design matrix A has columns for each coefficient.
// We solve A*x = b where b is the vector of measured errors.
// ---------------------------------------------------------------------------

void MountModel::fitModel()
{
    int n = m_observations.size();
    if (n < 4) {
        emit logMessage("Need at least 4 observations to fit model");
        return;
    }

    // Determine how many terms to fit based on observation count
    // With few observations, use reduced model to avoid overfitting
    int numTerms;
    if (n < 6) {
        numTerms = 4;  // IH, ID, MA, ME
    } else if (n < 10) {
        numTerms = 6;  // + CH, NP
    } else {
        numTerms = 8;  // + TF, FO (full model)
    }

    // Build the design matrix A (2n rows x numTerms cols) and observation vector b (2n)
    int rows = 2 * n;
    QVector<QVector<double>> A(rows, QVector<double>(numTerms, 0.0));
    QVector<double> b(rows, 0.0);

    for (int i = 0; i < n; i++) {
        const PointingObservation &obs = m_observations[i];
        double alt = obs.altDeg * DEG2RAD;
        double az = obs.azDeg * DEG2RAD;

        double cosAlt = std::cos(alt);
        double sinAlt = std::sin(alt);
        double tanAlt = (std::abs(cosAlt) > 1e-6) ? sinAlt / cosAlt : 0;
        double secAlt = (std::abs(cosAlt) > 1e-6) ? 1.0 / cosAlt : 0;
        double cosAz = std::cos(az);
        double sinAz = std::sin(az);

        int azRow = 2 * i;
        int altRow = 2 * i + 1;

        // Azimuth equation: dAz = IH + CH/cos(alt) + NP*tan(alt) + MA*cos(az)*tan(alt) + ME*sin(az)*tan(alt)
        A[azRow][0] = 1.0;                    // IH
        // ID column = 0 for azimuth
        if (numTerms >= 4) {
            // For reduced model, columns: IH=0, ID=1, MA=2, ME=3
            // For full model, columns: IH=0, ID=1, CH=2, NP=3, MA=4, ME=5, TF=6, FO=7
            if (numTerms == 4) {
                A[azRow][0] = 1.0;           // IH
                A[azRow][2] = cosAz * tanAlt; // MA
                A[azRow][3] = sinAz * tanAlt; // ME
            } else if (numTerms >= 6) {
                A[azRow][0] = 1.0;           // IH
                A[azRow][2] = secAlt;        // CH
                A[azRow][3] = tanAlt;        // NP
                A[azRow][4] = cosAz * tanAlt; // MA
                A[azRow][5] = sinAz * tanAlt; // ME
            }
        }

        // Altitude equation: dAlt = ID + TF*cos(alt) - MA*sin(az) + ME*cos(az) + FO*sin(alt)
        if (numTerms == 4) {
            A[altRow][1] = 1.0;              // ID
            A[altRow][2] = -sinAz;           // -MA
            A[altRow][3] = cosAz;            // ME
        } else if (numTerms >= 6) {
            A[altRow][1] = 1.0;              // ID
            A[altRow][4] = -sinAz;           // -MA
            A[altRow][5] = cosAz;            // ME
            if (numTerms >= 8) {
                A[altRow][6] = cosAlt;       // TF
                A[altRow][7] = sinAlt;       // FO
            }
        }

        // Measured errors (convert RA error to azimuth-like error)
        b[azRow] = obs.errorRaArcsec;
        b[altRow] = obs.errorDecArcsec;
    }

    // Solve via normal equations: x = (A^T A)^{-1} A^T b
    QVector<double> x;
    if (!solveLinearSystem(A, b, x)) {
        emit logMessage("Model fitting failed: singular matrix");
        return;
    }

    // Extract coefficients
    m_coeffs = ModelCoefficients();
    m_coeffs.numObservations = n;

    if (numTerms == 4) {
        m_coeffs.IH = x[0]; m_coeffs.ID = x[1]; m_coeffs.MA = x[2]; m_coeffs.ME = x[3];
    } else if (numTerms == 6) {
        m_coeffs.IH = x[0]; m_coeffs.ID = x[1]; m_coeffs.CH = x[2];
        m_coeffs.NP = x[3]; m_coeffs.MA = x[4]; m_coeffs.ME = x[5];
    } else {
        m_coeffs.IH = x[0]; m_coeffs.ID = x[1]; m_coeffs.CH = x[2];
        m_coeffs.NP = x[3]; m_coeffs.MA = x[4]; m_coeffs.ME = x[5];
        m_coeffs.TF = x[6]; m_coeffs.FO = x[7];
    }

    // Compute RMS residuals
    double sumRa2 = 0, sumDec2 = 0;
    for (int i = 0; i < n; i++) {
        double residAz, residAlt;
        computeResidual(m_observations[i], residAz, residAlt);
        sumRa2 += residAz * residAz;
        sumDec2 += residAlt * residAlt;
    }
    m_coeffs.rmsRaArcsec = std::sqrt(sumRa2 / n);
    m_coeffs.rmsDecArcsec = std::sqrt(sumDec2 / n);
    m_coeffs.rmsTotal = std::sqrt((sumRa2 + sumDec2) / (2 * n));

    m_modelValid = true;

    emit logMessage(QString("Model fit: %1 terms, %2 obs, RMS=%3\" (RA=%4\" Dec=%5\")")
                    .arg(numTerms).arg(n)
                    .arg(m_coeffs.rmsTotal, 0, 'f', 1)
                    .arg(m_coeffs.rmsRaArcsec, 0, 'f', 1)
                    .arg(m_coeffs.rmsDecArcsec, 0, 'f', 1));

    emit modelUpdated(m_coeffs);
}

void MountModel::computeResidual(const PointingObservation &obs,
                                  double &residAzArcsec, double &residAltArcsec) const
{
    double predAz, predAlt;
    predictModelError(obs.altDeg, obs.azDeg, predAz, predAlt);
    residAzArcsec = obs.errorRaArcsec - predAz;
    residAltArcsec = obs.errorDecArcsec - predAlt;
}

// ---------------------------------------------------------------------------
// Apply model correction to a target position
// ---------------------------------------------------------------------------

void MountModel::predictCorrection(double raDeg, double decDeg,
                                    double altDeg, double azDeg,
                                    double &corrRaDeg, double &corrDecDeg) const
{
    corrRaDeg = raDeg;
    corrDecDeg = decDeg;

    if (!m_modelValid) return;

    // Predict the error the mount would make at this position
    double dAzArcsec, dAltArcsec;
    predictModelError(altDeg, azDeg, dAzArcsec, dAltArcsec);

    // Apply the inverse correction: subtract the predicted error
    double cosDec = std::cos(decDeg * DEG2RAD);
    if (std::abs(cosDec) > 1e-6) {
        corrRaDeg = raDeg - dAzArcsec / (ARCSEC_PER_DEG * cosDec);
    }
    corrDecDeg = decDeg - dAltArcsec / ARCSEC_PER_DEG;
}

// ---------------------------------------------------------------------------
// Quaternion leveling: Wahba's problem via TRIAD method
//
// Given pairs of (commanded, actual) pointing directions, find the rotation
// quaternion Q such that actual = Q * commanded. Then to correct future
// GoTo commands, apply Q^{-1} to the target.
// ---------------------------------------------------------------------------

void MountModel::computeLevelingFromObservations()
{
    int n = m_observations.size();
    if (n < 3) return;

    // Use the first 3 observations that are well-separated
    // Convert commanded and actual directions to unit vectors
    QVector<double> cx(n), cy(n), cz(n); // commanded
    QVector<double> ax(n), ay(n), az(n); // actual

    for (int i = 0; i < n; i++) {
        dirToVec(m_observations[i].commandedRaDeg, m_observations[i].commandedDecDeg,
                 cx[i], cy[i], cz[i]);
        dirToVec(m_observations[i].actualRaDeg, m_observations[i].actualDecDeg,
                 ax[i], ay[i], az[i]);
    }

    // Davenport q-method: compute the 4x4 K matrix from the cross-covariance
    // B = sum_i w_i * a_i * c_i^T
    // For simplicity, use equal weights (w_i = 1/n)
    double B[3][3] = {{0}};
    for (int i = 0; i < n; i++) {
        B[0][0] += ax[i]*cx[i]; B[0][1] += ax[i]*cy[i]; B[0][2] += ax[i]*cz[i];
        B[1][0] += ay[i]*cx[i]; B[1][1] += ay[i]*cy[i]; B[1][2] += ay[i]*cz[i];
        B[2][0] += az[i]*cx[i]; B[2][1] += az[i]*cy[i]; B[2][2] += az[i]*cz[i];
    }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            B[i][j] /= n;

    double sigma = B[0][0] + B[1][1] + B[2][2]; // trace(B)

    // S = B + B^T
    double S[3][3];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            S[i][j] = B[i][j] + B[j][i];

    // Z = [B23-B32, B31-B13, B12-B21]
    double Z[3] = {
        B[1][2] - B[2][1],
        B[2][0] - B[0][2],
        B[0][1] - B[1][0]
    };

    // K matrix (4x4):
    // [ sigma,  Z^T    ]
    // [ Z,    S - sigma*I ]
    double K[4][4];
    K[0][0] = sigma;
    K[0][1] = Z[0]; K[0][2] = Z[1]; K[0][3] = Z[2];
    K[1][0] = Z[0]; K[2][0] = Z[1]; K[3][0] = Z[2];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            K[i+1][j+1] = S[i][j] - ((i == j) ? sigma : 0);

    // Find the eigenvector of K corresponding to the largest eigenvalue
    // using power iteration (K is symmetric 4x4, typically converges fast)
    double q[4] = {1, 0, 0, 0}; // initial guess (identity quaternion)
    for (int iter = 0; iter < 100; iter++) {
        double qnew[4] = {0, 0, 0, 0};
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                qnew[i] += K[i][j] * q[j];

        // Normalize
        double norm = 0;
        for (int i = 0; i < 4; i++) norm += qnew[i] * qnew[i];
        norm = std::sqrt(norm);
        if (norm < 1e-12) break;
        for (int i = 0; i < 4; i++) qnew[i] /= norm;

        // Check convergence
        double dot = 0;
        for (int i = 0; i < 4; i++) dot += q[i] * qnew[i];
        for (int i = 0; i < 4; i++) q[i] = qnew[i];
        if (std::abs(dot) > 1.0 - 1e-10) break;
    }

    // q = [w, x, y, z] is the optimal rotation quaternion
    // Ensure w > 0 (choose the canonical representation)
    if (q[0] < 0) { q[0] = -q[0]; q[1] = -q[1]; q[2] = -q[2]; q[3] = -q[3]; }

    m_levelingQ = Quaternion(q[0], q[1], q[2], q[3]);
    m_levelingQ.normalize();

    double tiltAngleDeg = m_levelingQ.angle() * RAD2DEG;
    emit logMessage(QString("Leveling quaternion: w=%1 x=%2 y=%3 z=%4 (tilt=%5 deg)")
                    .arg(m_levelingQ.w, 0, 'f', 6)
                    .arg(m_levelingQ.x, 0, 'f', 6)
                    .arg(m_levelingQ.y, 0, 'f', 6)
                    .arg(m_levelingQ.z, 0, 'f', 6)
                    .arg(tiltAngleDeg, 0, 'f', 3));

    emit levelingUpdated(m_levelingQ);
}

void MountModel::setLevelingQuaternion(const Quaternion &q)
{
    m_levelingQ = q;
    m_levelingQ.normalize();
    emit levelingUpdated(m_levelingQ);
}

void MountModel::applyLevelingCorrection(double &raDeg, double &decDeg,
                                          double /*altDeg*/, double /*azDeg*/) const
{
    // Convert target direction to unit vector
    double vx, vy, vz;
    dirToVec(raDeg, decDeg, vx, vy, vz);

    // Apply inverse rotation: Q^{-1} = Q* (conjugate)
    Quaternion qInv = m_levelingQ.conjugate();
    qInv.rotateVector(vx, vy, vz);

    // Convert back to RA/Dec
    vecToDir(vx, vy, vz, raDeg, decDeg);
}

// ---------------------------------------------------------------------------
// Linear least-squares solver via normal equations with Cholesky decomposition
// Solves A*x = b for x, where A is m x n (m >= n)
// Computes: A^T A x = A^T b
// ---------------------------------------------------------------------------

bool MountModel::solveLinearSystem(const QVector<QVector<double>> &A,
                                    const QVector<double> &b,
                                    QVector<double> &x)
{
    int m = A.size();
    if (m == 0) return false;
    int n = A[0].size();

    // Compute A^T A (n x n)
    QVector<QVector<double>> ATA(n, QVector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < m; k++)
                sum += A[k][i] * A[k][j];
            ATA[i][j] = sum;
            ATA[j][i] = sum;
        }
    }

    // Compute A^T b (n x 1)
    QVector<double> ATb(n, 0.0);
    for (int i = 0; i < n; i++) {
        double sum = 0;
        for (int k = 0; k < m; k++)
            sum += A[k][i] * b[k];
        ATb[i] = sum;
    }

    // Solve ATA * x = ATb using Gaussian elimination with partial pivoting
    // Augmented matrix [ATA | ATb]
    QVector<QVector<double>> aug(n, QVector<double>(n + 1));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            aug[i][j] = ATA[i][j];
        aug[i][n] = ATb[i];
    }

    // Forward elimination with partial pivoting
    for (int col = 0; col < n; col++) {
        // Find pivot
        int maxRow = col;
        double maxVal = std::abs(aug[col][col]);
        for (int row = col + 1; row < n; row++) {
            if (std::abs(aug[row][col]) > maxVal) {
                maxVal = std::abs(aug[row][col]);
                maxRow = row;
            }
        }

        if (maxVal < 1e-12) return false; // Singular

        // Swap rows
        if (maxRow != col)
            std::swap(aug[col], aug[maxRow]);

        // Eliminate below
        for (int row = col + 1; row < n; row++) {
            double factor = aug[row][col] / aug[col][col];
            for (int j = col; j <= n; j++)
                aug[row][j] -= factor * aug[col][j];
        }
    }

    // Back substitution
    x.resize(n);
    for (int i = n - 1; i >= 0; i--) {
        x[i] = aug[i][n];
        for (int j = i + 1; j < n; j++)
            x[i] -= aug[i][j] * x[j];
        x[i] /= aug[i][i];
    }

    return true;
}
