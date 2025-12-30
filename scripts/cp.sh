SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
echo $SCRIPT_DIR

SRC_DIR="${SCRIPT_DIR}/../src/gpu"
BUILD_DIR="${SCRIPT_DIR}/../build/"

echo $SRC_DIR
echo $BUILD_DIR
cp ${SRC_DIR}/*.glsl "${BUILD_DIR}"
