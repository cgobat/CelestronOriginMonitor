#pragma once

#include <QObject>
#include <QList>
#include <QDateTime>
#include <cmath>

// Quaternion for rigid-body rotation (leveling compensation)
struct Quaternion {
    double w = 1.0, x = 0.0, y = 0.0, z = 0.0;

    Quaternion() = default;
    Quaternion(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) {}

    Quaternion conjugate() const { return {w, -x, -y, -z}; }

    Quaternion operator*(const Quaternion &q) const {
        return {
            w*q.w - x*q.x - y*q.y - z*q.z,
            w*q.x + x*q.w + y*q.z - z*q.y,
            w*q.y - x*q.z + y*q.w + z*q.x,
            w*q.z + x*q.y - y*q.x + z*q.w
        };
    }

    void normalize() {
        double n = std::sqrt(w*w + x*x + y*y + z*z);
        if (n > 1e-12) { w /= n; x /= n; y /= n; z /= n; }
    }

    double norm() const { return std::sqrt(w*w + x*x + y*y + z*z); }

    // Rotation angle in radians
    double angle() const {
        double half = std::acos(std::min(1.0, std::abs(w)));
        return 2.0 * half;
    }

    static Quaternion fromAxisAngle(double ax, double ay, double az, double angleRad) {
        double half = angleRad * 0.5;
        double s = std::sin(half);
        double n = std::sqrt(ax*ax + ay*ay + az*az);
        if (n < 1e-12) return {1, 0, 0, 0};
        return {std::cos(half), s*ax/n, s*ay/n, s*az/n};
    }

    // Rotate a 3D vector by this quaternion: v' = q * v * q^{-1}
    void rotateVector(double &vx, double &vy, double &vz) const {
        Quaternion vq(0, vx, vy, vz);
        Quaternion result = (*this) * vq * conjugate();
        vx = result.x;
        vy = result.y;
        vz = result.z;
    }
};

// A single pointing observation
struct PointingObservation {
    QDateTime timestamp;
    double commandedRaDeg = 0;   // where mount thinks it's pointing (RA degrees)
    double commandedDecDeg = 0;  // (Dec degrees)
    double actualRaDeg = 0;      // plate-solve result (RA degrees)
    double actualDecDeg = 0;     // (Dec degrees)
    double altDeg = 0;           // altitude at observation time
    double azDeg = 0;            // azimuth at observation time
    double haDeg = 0;            // hour angle in degrees
    double errorRaArcsec = 0;    // (actual - commanded) * cos(dec) * 3600
    double errorDecArcsec = 0;   // (actual - commanded) * 3600
    double temperature = 0;
};

// TPOINT-style model coefficients for alt-az mount
struct ModelCoefficients {
    double IH = 0;    // Index error in azimuth (arcsec)
    double ID = 0;    // Index error in altitude (arcsec)
    double CH = 0;    // Collimation error (arcsec)
    double NP = 0;    // Non-perpendicularity of axes (arcsec)
    double MA = 0;    // Azimuth axis misalignment in azimuth (arcsec)
    double ME = 0;    // Azimuth axis misalignment in elevation (arcsec)
    double TF = 0;    // Tube flexure (arcsec)
    double FO = 0;    // Fork/optical axis flexure (arcsec)
    double rmsRaArcsec = 0;
    double rmsDecArcsec = 0;
    double rmsTotal = 0;
    int numObservations = 0;
};

class MountModel : public QObject
{
    Q_OBJECT
public:
    explicit MountModel(QObject *parent = nullptr);

    // Observation management
    void addObservation(const PointingObservation &obs);
    void clear();
    int observationCount() const { return m_observations.size(); }
    QList<PointingObservation> observations() const { return m_observations; }

    // Model fitting
    void fitModel();
    bool isModelValid() const { return m_modelValid; }
    ModelCoefficients coefficients() const { return m_coeffs; }

    // Apply corrections: given target RA/Dec and current Alt/Az,
    // returns corrected RA/Dec that compensates for pointing errors
    void predictCorrection(double raDeg, double decDeg,
                           double altDeg, double azDeg,
                           double &corrRaDeg, double &corrDecDeg) const;

    // Quaternion leveling
    void computeLevelingFromObservations();
    Quaternion levelingQuaternion() const { return m_levelingQ; }
    void setLevelingQuaternion(const Quaternion &q);
    void applyLevelingCorrection(double &raDeg, double &decDeg,
                                 double altDeg, double azDeg) const;

    // Model residuals for a single observation
    void computeResidual(const PointingObservation &obs,
                         double &residAzArcsec, double &residAltArcsec) const;

signals:
    void modelUpdated(const ModelCoefficients &coeffs);
    void levelingUpdated(const Quaternion &q);
    void logMessage(const QString &msg);

private:
    // Convert pointing direction to unit vector on the sphere
    static void dirToVec(double raDeg, double decDeg,
                         double &vx, double &vy, double &vz);
    static void vecToDir(double vx, double vy, double vz,
                         double &raDeg, double &decDeg);

    // Alt/Az to unit vector and back
    static void altAzToVec(double altDeg, double azDeg,
                           double &vx, double &vy, double &vz);
    static void vecToAltAz(double vx, double vy, double vz,
                           double &altDeg, double &azDeg);

    // TPOINT model prediction: predicted error in alt/az (arcsec)
    void predictModelError(double altDeg, double azDeg,
                           double &dAzArcsec, double &dAltArcsec) const;

    // Least-squares solver for small systems (Ax=b via normal equations)
    static bool solveLinearSystem(const QVector<QVector<double>> &A,
                                  const QVector<double> &b,
                                  QVector<double> &x);

    QList<PointingObservation> m_observations;
    ModelCoefficients m_coeffs;
    bool m_modelValid = false;
    Quaternion m_levelingQ;  // identity = no correction
};
