#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QSplitter>
#include "mainnotebook/mainnotebook.h"
#include "playlist/playlistwidget.h"
#include "playerbar.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    PlaylistWidget *playlistWidget;
    Phonon::MediaObject *mediaObject;

};

#endif // MAINWINDOW_H