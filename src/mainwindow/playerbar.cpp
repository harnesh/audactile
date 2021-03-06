#include "playerbar.h"

using namespace Phonon;

/// @brief Audactile's main toolbar class.
/// 
/// @param parent Parent widget
/// @param mediaObject MediaObject that will handle the files
/// @param audioOutput AudioOutput that will be linked to MediaObject
PlayerBar::PlayerBar(QWidget *parent, Phonon::MediaObject *mediaObject, Phonon::AudioOutput *audioOutput)
{
    mainMediaObject = mediaObject;
    setParent(parent);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);

    // Buttons
    playButton = new QToolButton(this);
    stopButton = new QToolButton(this);
    nextButton = new QToolButton(this);
    previousButton = new QToolButton(this);
    //fullScreenButton = new QToolButton(this);
    prefButton = new QToolButton(this);
    exitButton = new QToolButton(this);

    playButton->setIconSize(QSize(48, 48));
    stopButton->setIconSize(QSize(48, 48));
    nextButton->setIconSize(QSize(48, 48));
    previousButton->setIconSize(QSize(48, 48));
    //fullScreenButton->setIconSize(QSize(48, 48));
    prefButton->setIconSize(QSize(48, 48));
    exitButton->setIconSize(QSize(48, 48));

    playButton->setAutoRaise(true);
    stopButton->setAutoRaise(true);
    nextButton->setAutoRaise(true);
    previousButton->setAutoRaise(true);
    prefButton->setAutoRaise(true);
    //fullScreenButton->setAutoRaise(true);
    exitButton->setAutoRaise(true);

    playButton->setIcon(IconFactory::fromTheme("media-playback-start"));
    stopButton->setIcon(IconFactory::fromTheme("media-playback-stop"));
    nextButton->setIcon(IconFactory::fromTheme("media-skip-forward"));
    previousButton->setIcon(IconFactory::fromTheme("media-skip-backward"));
    prefButton->setIcon(IconFactory::fromTheme("preferences-other"));
    //fullScreenButton->setIcon(IconFactory::fromTheme("view-fullscreen"));
    exitButton->setIcon(IconFactory::fromTheme("application-exit"));

    stopButton->setDisabled(true);

    // Signals from buttons
    connect(playButton, SIGNAL(clicked()), this, SLOT(handlePlayButton()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(handleStopButton()));
    connect(nextButton, SIGNAL(clicked()), this, SIGNAL(nextButtonClicked()));
    connect(previousButton, SIGNAL(clicked()), this, SIGNAL(previousButtonClicked()));
    connect(prefButton, SIGNAL(clicked()), this, SLOT(openSettings()));
    connect(exitButton, SIGNAL(clicked()), this, SLOT(exitApplication()));
    //connect(fullScreenButton, SIGNAL(clicked()), this, SIGNAL(toggleFullScreen()));

    // Vertical box with slider and current song time labels
    QFrame *songPositionWidget = new QFrame(this);
    songPositionWidget->setFrameShadow(QFrame::Sunken);
    songPositionWidget->setFrameShape(QFrame::StyledPanel);
    songPositionWidget->setMidLineWidth(4);


    // Horizontal box with current song position labels
    QWidget *songPositionLabelsWidget = new QWidget(this);
    QHBoxLayout *songPositionLabelsHBox = new QHBoxLayout(songPositionLabelsWidget);
    currentSongPosition = new QLabel(tr("--:--"));
    remainingSongPosition = new QLabel(tr("--:--"));
    currentSongInfo = new QLabel(tr(""));

    // Define song info to be bold
    QFont songInfoFont = currentSongInfo->font();
    songInfoFont.setBold(true);
    songInfoFont.setPointSize(songInfoFont.pointSize() + 2);
    currentSongInfo->setFont(songInfoFont);

    // Add labels
    songPositionLabelsHBox->addWidget(currentSongPosition, 1, Qt::AlignLeft);
    songPositionLabelsHBox->addWidget(currentSongInfo, 1, Qt::AlignCenter);
    songPositionLabelsHBox->addWidget(remainingSongPosition, 1, Qt::AlignRight);
    songPositionLabelsWidget->setLayout(songPositionLabelsHBox);

    // Set background for song position widget
    QPalette palette = songPositionWidget->palette();
    QLinearGradient songPositionBgGradient(1.0, 1.0, 1.0, 40.0);
    songPositionBgGradient.setColorAt(0.0, QColor(250,250,230));
    songPositionBgGradient.setColorAt(1.0, QColor(220,220,200));
    palette.setBrush(songPositionLabelsWidget->backgroundRole(), QBrush(songPositionBgGradient));
    palette.setColor(songPositionLabelsWidget->foregroundRole(), QColor(100,100,100));
    songPositionWidget->setPalette(palette);
    songPositionWidget->setAutoFillBackground(true);

    QVBoxLayout *songPositionVBox = new QVBoxLayout(songPositionWidget);
    songPositionSlider = new SeekSlider(mainMediaObject, this);
    songPositionSlider->setOrientation(Qt::Horizontal);
    songPositionVBox->addWidget(songPositionLabelsWidget);
    songPositionVBox->addWidget(songPositionSlider);
    songPositionWidget->setLayout(songPositionVBox);

    // Volume button
    Phonon::VolumeSlider *volumeSlider = new Phonon::VolumeSlider(audioOutput, this);
    volumeSlider->setMaximumWidth((int)floor(0.2*(window()->width())));

    QHBoxLayout *playerBarLayout = new QHBoxLayout(this);
    playerBarLayout->addWidget(playButton);
    playerBarLayout->addWidget(stopButton);
    playerBarLayout->addWidget(Separator::verticalSeparator(this));
    playerBarLayout->addWidget(previousButton);
    playerBarLayout->addWidget(nextButton);
    playerBarLayout->addWidget(Separator::verticalSeparator(this));
    playerBarLayout->addWidget(songPositionWidget);
    playerBarLayout->addWidget(Separator::verticalSeparator(this));
    playerBarLayout->addWidget(volumeSlider);
    playerBarLayout->addWidget(Separator::verticalSeparator(this));
    //playerBarLayout->addWidget(fullScreenButton);
    playerBarLayout->addWidget(prefButton);
    playerBarLayout->addWidget(exitButton);

    setLayout(playerBarLayout);

    // Signals from media source
    connect(mainMediaObject, SIGNAL(tick(qint64)), this, SLOT(updateSongPosition()));
    connect(mainMediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(handleState(Phonon::State)));
    connect(mainMediaObject, SIGNAL(finished()), this, SLOT(finish()));

    show();
}



/// @brief Update current song position and remaining song time on playerbar.
void PlayerBar::updateSongPosition() {
    int secs, mins;
    QString qStr;

    /*
     * Get the remaining time. If we have more than 1000 ms, we
     * can update the time. The last second must not update the
     * playerbar, since after that the timer will become "--:--".
     */
    qint64 rem = mainMediaObject->remainingTime();
    if (rem > 1000) {
        qint64 cur = mainMediaObject->currentTime();

        songPositionSlider->setEnabled(true);

        secs = (rem / 1000) % 60;
        mins = (rem / 1000) / 60;
        remainingSongPosition->setText("-" + QString::number(mins) + ":" + qStr.sprintf("%02d", secs));

        secs = (cur / 1000) % 60;
        mins = (cur / 1000) / 60;
        currentSongPosition->setText(QString::number(mins) + ":" + qStr.sprintf("%02d", secs));
    }

}


/// @brief Handle the state change in the MediaObject.
/// 
/// @param newState New state
/// @param oldState Old state
void PlayerBar::handleState(Phonon::State newState) {
    // Handle play button
    if (newState == Phonon::PlayingState) {
        playButton->setIcon(IconFactory::fromTheme("media-playback-pause"));
    }
    else if (newState == Phonon::StoppedState) {
        playButton->setIcon(IconFactory::fromTheme("media-playback-start"));
    }
    else if (newState == Phonon::PausedState) {
        playButton->setIcon(IconFactory::fromTheme("media-playback-start"));
    }

    // Handle stop button
    if (newState <= Phonon::StoppedState) {
        stopButton->setDisabled(true);
    }
    else {
        stopButton->setEnabled(true);
    }
}

// TODO: deliver the play/stop buttons callback to PlaylistWidget using signals.

/// @brief Callback for play button
void PlayerBar::handlePlayButton() {
    if (mainMediaObject->state() == Phonon::PlayingState) {
        mainMediaObject->pause();
    }
    else if (mainMediaObject->currentSource().type() != MediaSource::Empty) {
        mainMediaObject->play();
    }
    updateSongPosition();
}

/// @brief Callback for stop button
void PlayerBar::handleStopButton() {
    if (mainMediaObject->state() > Phonon::StoppedState) {
        resetDisplay();
        mainMediaObject->stop();
    }
}

/// @brief Update the song information in the bar
/// 
/// @param newSongInformation
void PlayerBar::updateSongInformation(QMap<QString, QString>newSongInformation) {
    currentSongInfo->setText(newSongInformation.value("artist") + " - " + newSongInformation.value("title"));
}

/// @brief Reset information to its initial state
void PlayerBar::resetDisplay() {
    qDebug("resetDisplay");
    currentSongPosition->setText(tr("--:--"));
    remainingSongPosition->setText(tr("--:--"));
    currentSongInfo->setText(tr(""));
}

/// @brief Commands to execute when the music ends
void PlayerBar::finish() {
    resetDisplay();
    mainMediaObject->stop();
}


/// @brief Quit application
void PlayerBar::exitApplication() {
    exit(0);
}

/// @brief Open settings
void PlayerBar::openSettings() {
    SettingsDialog *dialog = new SettingsDialog(parentWidget());
}

/// @brief Set the correct button to screen button
void PlayerBar::handleWindowStateChange(Qt::WindowStates windowState) {
    if (windowState == Qt::WindowFullScreen) {
        fullScreenButton->setIcon(IconFactory::fromTheme("view-restore"));
    }
    else {
        fullScreenButton->setIcon(IconFactory::fromTheme("view-fullscreen"));
    }
}
