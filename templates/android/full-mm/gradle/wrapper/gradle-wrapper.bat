@ECHO OFF
SET JAR=%~dp0gradle-wrapper.jar
IF NOT EXIST "%JAR%" ECHO Missing gradle-wrapper.jar. Run scripts\fetch_gradle_wrapper.ps1 & EXIT /B 1
java -jar "%JAR%" %*
