# CouchCoopOnline
This is a tool that allows the sharing of limited keyboard space across a network(currently for up to 3 other players not including the host).  It was originally designed for the game Crawl, and hence the file names.
This does not provide screen sharing as there is a much better alternative, I use a live stream through OBS that is sent to an RTMP server hosted on my Raspberry Pi(steps described here https://obsproject.com/forum/resources/how-to-set-up-your-own-private-rtmp-server-using-nginx.50/) that I then have my players view with it with ffplay using the command
```
ffplay -fflags nobuffer rtmp://[Your link] -loglevel verbose
```
However, that is not to say I would not be open to, and willing to contribute to someone attempting to add screen and audio sharing(preferably with the ability to target only the sound source of the chosen game) to this project.  Screen sharing with this would likely best be done by only sending updated pixels in a matrix, rather than traditional video files.
