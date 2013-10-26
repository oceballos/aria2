#!/bin/sh

UA=`youtube-dl --dump-user-agent`
#TMPDIR=`mktemp -d`
mkdir videoInfo
TMPDIR="videoInfo"
COOKIES="$TMPDIR/cookies"

#trap "rm -rf $TMPDIR" 0

ARIA_DNS_FLAGS=""
aria2c -h#all|grep -- '--async-dns' >/dev/null 2>&1
if [ $? -eq 0 ]
then
	ARIA_DNS_FLAGS="--async-dns=false"
fi

# Uso youtbe-dl para crear y guardar la cookie que usare con aria2c
youtube-dl -o "[%(upload_date)s][%(id)s] %(title)s (by %(uploader)s).%(ext)s" --get-url --get-filename --cookies=${COOKIES} "$@" > ${TMPDIR}/video_data

#EJEMPLO DE METALINK
#<?xml version="1.0" encoding="UTF-8"?> 
#<metalink version="3.0" xmlns="http://www.metalinker.org/">
#    <publisher>
#        <name>Petar Marić</name>
#        <url>http://www.petarmaric.com/</url>
#    </publisher>
#    <description>Download TED talks with English subtitles encoded in high quality</description>
#    <tags>TED, download, video</tags>
#    <files>
#        <file name="Christopher deCharms - A look inside the brain in real time.mp4">
#            <resources>
#                <url type="http">http://download.ted.com/talks/ChristopherdeCharms_2008-480p-en.mp4?apikey=TEDDOWNLOAD</url>
#            </resources>
#        </file>
#    </files>
#</metalink>


echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" > video.metalink
echo "<metalink version=\"3.0\" xmlns=\"http://www.metalinker.org/\">" >> video.metalink
echo "    <publisher>" >> video.metalink 
echo "        <name>Youtube.com</name>">> video.metalink
echo "        <url>$@</url>">> video.metalink
echo "    </publisher>">> video.metalink
echo "    <description>Download youtbe videos using aria2c</description>" >>video.metalink
echo "    <tags>youtube, download, video</tags>" >>video.metalink
echo "    <files>">> video.metalink

VIDEONAME=`cat ${TMPDIR}/video_data | grep "\["`
echo "        <file name=\"$VIDEONAME\">">> video.metalink
#echo "     <size></size>">> video.metalink
#echo "     <verification></verification>">> video.metalink
echo "            <resources>">> video.metalink

for i in `seq 1 3`
do
   URIAUX=`cat ${TMPDIR}/video_data |grep http:// | sed 's/http:\/\/r[0-9]/http:\/\/r'$i'/'`
   SHORTURI=`surl -c$URIAUX -stinyurl.com`;
   echo "                <url type=\"http\">$SHORTURI</url>" >> video.metalink
done
echo "     </resources>">> video.metalink
echo "   </file>" >> video.metalink
echo " </files>" >> video.metalink
echo "</metalink>">> video.metalink

while read URL && read FILENAME
do
	CLEANED_FILENAME=`echo "${FILENAME}" | tail -n 1 | tr ":\"" ";'" | tr -d "\\\/*?<>|"`

	echo "$CLEANED_FILENAME"
	#aria2c $ARIA_DNS_FLAGS -c -j 3 -x 3 -s 3 -k 1M --load-cookies="$COOKIES" -U "$UA" -o "$CLEANED_FILENAME" "$URL"
	 aria2c $ARIA_DNS_FLAGS -c -j 3 -x 3 -s 3 -k 1M --load-cookies="$COOKIES" -U "$UA" -o "$CLEANED_FILENAME" -M video.metalink -l LogMETA
done < $TMPDIR/video_data
