#!/bin/sh

uri=$1
#regex='^[[http:\/\/]?[?:youtu\.be\/|[?:[a-z]{2,3}\.]?youtube\.com\/v\/][[\w-]{11}].*|http:\/\/[?:youtu\.be\/|(?:[a-z]{2,3}\.)?youtube\.com\/watch(?:\?|#\!)v=][[\w-]{11}].*]$'
#regex2=[[ "$newfile" =~ ([[\ {\(-]*(www\.)?(torrentday\.com|torrenting\.com|spastikustv|speed\.cd|moviesp2p\.com|publichd\.org|publichd|scenetime\.com|kingdom-release)[]\ }\)-]*) ]]
#regex3='(http\:\/\/)?(www)?\.?(youtube)\.?(com\/)(watch\?)(v\=)[a-zA-Z0-9\_]{11}'
regex4='((http\:\/\/)|(https\:\/\/))?((www\.)|(w2\.)|(w3\.))?((youtube)\.(com\/)|(youtu)\.(be\/))(watch\?)(v\=)[a-zA-Z0-9\_\-]{11}'


if [[ "$uri" =~ $regex4 ]] ;
then
    echo "OK"
else
    echo "not OK"
fi