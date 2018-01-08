#include <QDateTime>
#include <QUrl>
#include <QCryptographicHash>
#include <QMapIterator>
#include "tts_responsivevoice.h"
#include "log.h"

Q_EXPORT_PLUGIN2(tts_responsivevoice, TTSresponsivevoice)

TTSresponsivevoice::TTSresponsivevoice():TTSInterface("responsivevoice", "responsivevoice")
{
	voiceList.insert("fr", "fr");
}

TTSresponsivevoice::~TTSresponsivevoice()
{
}

QByteArray TTSresponsivevoice::CreateNewSound(QString text, QString voice, bool forceOverwrite)
{
	QEventLoop loop;

	if(!voiceList.contains(voice))
		voice = "fr";

	// Check (and create if needed) output folder
	QDir outputFolder = ttsFolder;
	if(!outputFolder.exists(voice))
		outputFolder.mkdir(voice);

	if(!outputFolder.cd(voice))
	{
		LogError(QString("Cant create TTS Folder : %1").arg(ttsFolder.absoluteFilePath(voice)));
		return QByteArray();
	}

	// Compute fileName
	QString fileName = QCryptographicHash::hash(text.toAscii(), QCryptographicHash::Md5).toHex().append(".mp3");
	QString filePath = outputFolder.absoluteFilePath(fileName);

	if(!forceOverwrite && QFile::exists(filePath))
		return ttsHTTPUrl.arg(voice, fileName).toAscii();

    // Fetch MP3
    QHttp http("code.responsivevoice.org",QHttp::ConnectionModeHttps,443);
    QObject::connect(&http, SIGNAL(done(bool)), &loop, SLOT(quit()));

    QByteArray ContentData;
    ContentData += "?t=" + QUrl::toPercentEncoding(text) + "&tl=fr";

    QHttpRequestHeader Header;
    Header.addValue("Host", "code.responsivevoice.org");

    Header.setContentLength(ContentData.length());
    Header.setRequest("GET", "/getvoice.php?tl=fr&t="+QUrl::toPercentEncoding(text), 1, 1);
    LogInfo(QString("send : %1").arg(Header.toString()));
    http.request(Header, ContentData);
    LogInfo(QString("reponse size: %1").arg(http.bytesAvailable()));
    //LogInfo(QString("reponse : %1").arg(http.readAll()));
    loop.exec();


	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly))
	{
		LogError("Cannot open sound file for writing");
		return QByteArray();
	}
	file.write(http.readAll());
	file.close();
	return ttsHTTPUrl.arg(voice, fileName).toAscii();
}

