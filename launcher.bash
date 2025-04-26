#! /usr/bin/env bash

readonly MY_PATH=$(realpath "$0")
echo -e "Raw Path: $0\nResolved Path: $MY_PATH"
MY_NAME=
logger() {
	if [ "$2" ]; then
		echo "$MY_NAME: $1" 1>&2
		exit 1
	else
		echo "$MY_NAME: $1"
	fi
}
if [[ "$MY_PATH" =~ ^(.*)/([^/]+)$ ]]; then
	readonly MY_DIR="${BASH_REMATCH[1]}" MY_NAME="${BASH_REMATCH[2]}"
	logger "Directory: ${MY_DIR}"
else
	readonly MY_DIR="$HOME" MY_NAME="$0"
	logger "Failed to get dir" 1
fi

# TODO: Support Game Switching w/ arguments
DATA_DIR="$MY_DIR/data"
if [ -d "$DATA_DIR" ]; then
	if [ -d "$DATA_DIR/AITD1_CD" ]; then
		readonly DATA_DIR="$DATA_DIR/AITD1_CD"
	elif [ -d "$DATA_DIR/AITD1_FLOPPY" ]; then
		readonly DATA_DIR="$DATA_DIR/AITD1_FLOPPY"
	elif [ -d "$DATA_DIR/AITD2" ]; then
		readonly DATA_DIR="$DATA_DIR/AITD2"
	elif [ -d "$DATA_DIR/AITD3" ]; then
		readonly DATA_DIR="$DATA_DIR/AITD3"
	elif [ -d "$DATA_DIR/TIMEGATE" ]; then
		readonly DATA_DIR="$DATA_DIR/TIMEGATE"
	else
		logger "Game data directory found but no game folders were found. Expected a \"AITD1_CD\", \"AITD2\", \"AITD3\", or \"TIMEGATE\" folder. Assuming game data is in placed directly in the data directory."
	fi
	logger "Game data directory found: $DATA_DIR"
else
	logger "Game data Directory \"$DATA_DIR\" not found!" 1
fi

readonly EXE_DIR="$MY_DIR/build/Fitd"
if [ -d "$EXE_DIR" ]; then
	logger "Executable directory found: $EXE_DIR"
else
	logger "Executable directory \"$EXE_DIR\" not found!" 1
fi

EXE_NAME=
FALLBACK_EXE_NAME=
case "$OSTYPE" in
	(linux*)
		readonly EXE_NAME="Fitd" FALLBACK_EXE_NAME="Fitd.exe"
		logger "Platform is Linux; searching for \"$EXE_DIR$EXE_NAME\"..."
		;;
	(*win*)
		readonly EXE_NAME="Fitd.exe" FALLBACK_EXE_NAME="Fitd"
		logger "Platform is Windows; searching for \"$EXE_DIR$EXE_NAME\"..."
		;;
	(*mac*)
		readonly EXE_NAME="Fitd" FALLBACK_EXE_NAME="Fitd.exe"
		logger "Platform is macOS; searching for \"$EXE_DIR$EXE_NAME\"..."
		;;
	(*)
		readonly EXE_NAME="Fitd" FALLBACK_EXE_NAME="Fitd.exe"
		logger "Unknown platform; assuming Linux/macOS & searching for \"$EXE_DIR$EXE_NAME\"..."
		;;
esac

# Use -x to test for executability
if [ -e "$EXE_DIR/$EXE_NAME" ]; then
	logger "Executable $EXE_DIR/$EXE_NAME found! Launching..."
	cd "$DATA_DIR" || logger "Couldn't change directory to $DATA_DIR" 1
	exec "$EXE_DIR/$EXE_NAME"
else
	logger "Executable \"$EXE_DIR/$EXE_NAME\" not found; falling back to $EXE_DIR/$FALLBACK_EXE_NAME"
	if [ -e "$EXE_DIR/$FALLBACK_EXE_NAME" ]; then
		logger "Executable $EXE_DIR/$FALLBACK_EXE_NAME found! Attempting to launch..."
		cd "$DATA_DIR" || logger "Couldn't change directory to $DATA_DIR" 1
		exec "$EXE_DIR/$FALLBACK_EXE_NAME"
	else
		logger "Executable \"$EXE_DIR/$FALLBACK_EXE_NAME\" not found!" 1
	fi
fi

exit $?