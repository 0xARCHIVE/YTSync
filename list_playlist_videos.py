#!/usr/bin/python3
import sys
import youtube_dl

def get_playlist_videos(playlist_id):
	options = {
		'extract_flat':True,
		'simulate':True,
		'quiet':True,
	}

	output = []

	with youtube_dl.YoutubeDL(options) as ytdl:
		try:
			info = ytdl.extract_info("https://www.youtube.com/playlist?list=" + str(playlist_id))
			[output.append(i['id']) for i in info['entries']]
		except:
			return "0"

	return "\n".join(output)

if __name__ == "__main__":
	if (len(sys.argv) != 2):
		sys.exit()
	print(get_playlist_videos(sys.argv[1]))
