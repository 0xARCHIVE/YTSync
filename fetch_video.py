#!/usr/bin/python3
import os
import sys
import youtube_dl

def fetch_video(video_id,dl_location):
	options = {
		'quiet':True,
		'format':'bestaudio/best',
		'extractaudio':True,
		'audioformat':'mp3',
		'outtmpl':os.path.join(dl_location,"./") + u'%(id)s.%(ext)s',
		'nooverwrites':True,
		'writethumbnail':True,
		'nocheckcertificate':True,
		'postprocessors':[{
			'key':'FFmpegExtractAudio',
			'preferredcodec':'mp3',
			'preferredquality':'192'},
			{'key':'EmbedThumbnail'},
			{'key':'FFmpegMetadata'}]
		}

	with youtube_dl.YoutubeDL(options) as ytdl:
		try:
			ytdl.download(["https://www.youtube.com/watch?v=" + str(video_id)])
		except:
			return "0"

	return "1"

if __name__ == "__main__":
	if (len(sys.argv) != 3):
		sys.exit()
	print(fetch_video(sys.argv[1],sys.argv[2]))
