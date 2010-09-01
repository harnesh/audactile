#ifndef MUSIC_H
#define MUSIC_H

#include <QUrl>
#include <taglib/tag.h>
#include <taglib/fileref.h>

class Music
{
public:
    Music(QUrl fileUrl);
    QUrl getFileUrl();
    QString getArtist();
    QString getTitle();
    QString getAlbum();
    unsigned int getTrackNumber();
    QString getSongInfo();
    unsigned int getDuration();
    bool isValid();

private:
    bool valid;
    QUrl fileUrl;
    unsigned int trackNumber;
    QString artist;
    QString title;
    QString album;
    unsigned int duration;

};

#endif // MUSIC_H
