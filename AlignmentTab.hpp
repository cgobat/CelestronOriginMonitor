#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include "AlignmentController.hpp"

class AlignmentTab : public QWidget {
    Q_OBJECT
public:
    explicit AlignmentTab(AlignmentController *controller, QWidget *parent = nullptr);

private slots:
    void onStateChanged(AlignmentController::AlignState state);
    void onLogMessage(const QString &message);
    void onPlateSolveCompleted(const PlateSolveResult &result);
    void onModelReady(const ModelCoefficients &coeffs);
    void onStarStatusesUpdated();
    void onLevelingUpdated(const Quaternion &q);

    void onStartClicked();
    void onAbortClicked();
    void onOneShotClicked();
    void onClearModelClicked();

private:
    void setupUI();

    AlignmentController *m_controller;

    // Controls
    QPushButton *m_startButton;
    QPushButton *m_abortButton;
    QPushButton *m_oneShotButton;
    QPushButton *m_clearModelButton;
    QSpinBox *m_numStarsSpin;
    QLabel *m_stateLabel;
    QLabel *m_progressLabel;

    // Plate solve results
    QLabel *m_solvedRaLabel;
    QLabel *m_solvedDecLabel;
    QLabel *m_solveErrorLabel;
    QLabel *m_orientationLabel;
    QLabel *m_pixscaleLabel;
    QLabel *m_fieldSizeLabel;
    QLabel *m_solveTimeLabel;
    QLabel *m_numStarsLabel;

    // Mount model
    QTableWidget *m_coeffTable;
    QLabel *m_rmsLabel;
    QLabel *m_obsCountLabel;

    // Leveling
    QLabel *m_quaternionLabel;
    QLabel *m_tiltAngleLabel;

    // Star status table
    QTableWidget *m_starTable;

    // Log
    QTextEdit *m_logView;
};
