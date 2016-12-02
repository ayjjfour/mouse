protoc -I=./ --cpp_out=./ GameMessage.proto

lua proto.lua

echo off
rem xcopy "GameMessageTags.xml" "../NiuNiu/Resources/GameMessageTags.xml" /Y
rem xcopy "GameMessage.json" "../NiuNiu/Resources/GameMessage.json" /Y
rem xcopy "GameMessageTags.lua" "../NiuNiu/Resources/Script/GameMessageTags.lua" /Y
echo on

pause