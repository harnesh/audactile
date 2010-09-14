#include "lyricswidget.h"

LyricsWidget::LyricsWidget(QWidget *parent) : AbstractContainer(parent)
{
    setHtml("<div class=\"title\">No music being listened!</div>");

    // Try to fetch lyrics
    lyricsProvider = new LyricsDownloader();
    connect(lyricsProvider, SIGNAL(lyricsReady(QWebElement)), this, SLOT(showLyrics(QWebElement)));
}

void LyricsWidget::songInformationUpdated(QMap<QString, QString> newContextInformation) {
    lastRequest = newContextInformation;
    QString html("<div class=\"title\">" + newContextInformation.value("artist") + " - " + newContextInformation.value("title") +  "</div>");
    setHtml(html);
    lyricsProvider->getLyrics(newContextInformation.value("artist"), newContextInformation.value("title"));
}

void LyricsWidget::showLyrics(QWebElement webElement) {
    // FIXME: if you change song before the lyrics are loaded, the lyrics shown are for the previous song
    qDebug("Trying to show lyrics...");
    QString html("<div class=\"title\">" + lastRequest.value("artist") + " - " + lastRequest.value("title") +  "</div>");
    setHtml(html + "<p>" + webElement.toOuterXml() + "</p>");

    // Code to try to remove some tags
    QString code = "$('object').remove()";
    contentView->page()->mainFrame()->evaluateJavaScript(code);
}
