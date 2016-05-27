#!/bin/bash

set -e

[ -n "$1" ] || { echo Usage: $0 udpipe_version >&2; exit 1; }
udpipe="$1"

dir=udpipe-"$udpipe"-bin
[ -d "$dir" ] && { echo Release $dir already exists >&2; exit 1; }

mkdir "$dir"
mkdir "$dir/src_lib_only"
make -C ../src_lib_only udpipe.cpp
cp ../src_lib_only/udpipe.[ch]* "$dir/src_lib_only"

EXES=(udpipe)
make -C ../src clean
make -C ../bindings/csharp clean
make -C ../bindings/csharp udpipe_csharp.cpp
make -C ../bindings/java clean
make -C ../bindings/java udpipe.jar
for b in linux32:linux-gcc-32.sh linux64:linux-gcc-64.sh win32:visual-cpp-32.sh win64:visual-cpp-64.sh osx:remote-osx-clang-32+64.sh\ manfred; do
  name="${b%%:*}"
  make="/net/projects/cpp_builtem/compilers/make-${b#*:}"

  # Choose make and targets
  case $name in
    linux*) targets=(${EXES[@]});        csharp=libudpipe_csharp.so;    java=libudpipe_java.so;;
    win*)   targets=(${EXES[@]/%/.exe}); csharp=udpipe_csharp.dll;      java=udpipe_java.dll;;
    osx*)   targets=(${EXES[@]});        csharp=libudpipe_csharp.dylib; java=libudpipe_java.dylib;;
  esac

  # Compile UDPipe
  export RSYNC_ARGS=--exclude=training/\*
  mkdir "$dir/bin-$name"
  (cd .. && $make -C src -j3 MODE=release ${targets[@]})
  (cd .. && $make -C bindings/csharp -j3 MODE=release $csharp)
  (cd .. && $make -C bindings/java -j3 MODE=release JAVA_HOME=../../releases/java $java)

  # Copy files
  cp ${targets[@]/#/../src/} "$dir/bin-$name"

  mkdir "$dir/bin-$name/csharp" "$dir/bin-$name/java"
  cp -a ../bindings/csharp/Ufal ../bindings/csharp/$csharp "$dir/bin-$name/csharp"
  cp ../bindings/java/udpipe.jar ../bindings/java/$java "$dir/bin-$name/java"
done

make -C ../src clean
rsync -a --exclude .gitignore ../src/ "$dir/src"

for lang in ../bindings/*/Makefile; do make -C "`dirname $lang`" clean; done
rsync -a --exclude .gitignore ../bindings/ "$dir/bindings"

make -C ../doc install
cp ../INSTALL ../MANUAL* ../README ../AUTHORS ../LICENSE ../CHANGES "$dir"
make -C ../doc manual.pdf
cp ../doc/manual.pdf "$dir"/MANUAL.pdf
make -C ../doc clean

echo All done
