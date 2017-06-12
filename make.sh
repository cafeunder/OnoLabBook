#!/bin/sh
#
# ----------------
# ./make.sh build
# env CONTAINER_VERSION="debug" BUILD_SKIP="1" ./make.sh build
# ----------------
# 本をビルドする
#   * 結果は tmp/ 以下に各種ログと共に出力
# 環境変数 $CONTAINER_VERSION を読む
#   * ビルド用コンテナのタグを指定
#   * "debug" なら docker pull を省略
#   * 無指定なら circle.yml を基に決定
# 環境変数 $BUILD_SKIP も読む
#   * 1 以上で超簡易処理へ
#     * 配布用PDF/入稿用PDF/EPUB を作らない
#     * 誤字脱字チェックもしない
#     * Dockerコンテナをpullしない
#   * 無指定なら circle.yml を基に決定
#
#
# -----------------------
# ./make.sh install HOGE
# -----------------------
# HOGE章を追加する
#   * articles/HOGE.re を作成        (HOGE.re が存在しない場合)
#   * src/catalog.yml へ HOGE を追加 (HOGE が存在しない場合)
#   * src/images/HOGE を作成         (src/images/HOGE が存在しない場合)
#


#
# build
#
build() {
	set +u
	if [ ! -n "$BUILD_SKIP" ]; then BUILD_SKIP=$(grep "^\s*BUILD_SKIP\s*:\w*\s*" circle.yml | tr -d ' ' | cut -d ':' -f2 | sed -e 's/"//g'); fi
	if [ ! -n "$CONTAINER_VERSION" ]; then CONTAINER_VERSION=$(grep "^\s*CONTAINER_VERSION\s*:\w*\s*" circle.yml | tr -d ' ' | cut -d ':' -f2 | sed -e 's/"//g');
	fi
	set -u

	container="lrks/desk:${CONTAINER_VERSION}"
	if [ ${#CONTAINER_VERSION} -eq 64 ]; then container="lrks/desk@sha256:${CONTAINER_VERSION}"; fi
	if [ "$CONTAINER_VERSION" != "debug" -a "$BUILD_SKIP" -lt 1 ]; then sudo docker pull "$container"; fi

	mkdir -p artifacts tmp
	mkdir -p src/tmp
	cd tmp/ || return
	rm -rf ./*.log origin.pdf body.pdf book.pdf book.epub cover.png back.png
	cd ../ || return
	cp -f artifacts/cover.png artifacts/back.png src/tmp/ 2> /dev/null
	echo "0" > src/tmp/pdfcode
	echo "0" > src/tmp/epubcode

	cmd="cd /work"
	if [ "$BUILD_SKIP" -lt 1 ]; then
		cmd="${cmd} && check.sh 2> /dev/null | tee redpen.log"
		cmd="${cmd} && mv -f redpen*.log articles/redpen*.log tmp/"
	fi
	cmd="${cmd} && compile.rb pdf tmp/pdfcode > tmp/latex.log"
	cmd="${cmd} && mv -f book.pdf tmp/origin.pdf"
	if [ "$BUILD_SKIP" -lt 1 ]; then
		cmd="${cmd} && cd tmp/"
		cmd="${cmd} && print.sh origin.pdf body.pdf > /dev/null"
		cmd="${cmd} && if [ ! -e cover.png ]; then convert -size 850x1200 -background gray -fill red -gravity center label:COVER cover.png; fi"
		cmd="${cmd} && if [ ! -e back.png ]; then convert -size 850x1200 -background gray -fill red -gravity center label:BACK back.png; fi"
		cmd="${cmd} && cd ../"
		cmd="${cmd} && compile.rb pubpdf tmp/pubpdfcode > tmp/publatex.log"
		cmd="${cmd} && mv -f book.pdf tmp/pubpdf.pdf"
		cmd="${cmd} && cd tmp/"
		cmd="${cmd} && binding.sh cover.png pubpdf.pdf back.png book.pdf > /dev/stdout"
		cmd="${cmd} && cd ../"
		cmd="${cmd} && convert -resize 590x750 tmp/cover.png images/epub-cover.png"
		cmd="${cmd} && compile.rb epub tmp/epubcode > tmp/epub.log"
		cmd="${cmd} && mv -f book.epub tmp/book.epub"
	fi
	cmd="(${cmd}); echo \$? > /work/tmp/retcode"

	set +eu
	if [ -n "$CIRCLE_BRANCH" ]; then
		sudo docker run -v "$(pwd)/src/:/work" "$container" /bin/ash -c "${cmd}" | tee tmp/build.log
	else
		sudo docker run --rm -v "$(pwd)/src/:/work" "$container" /bin/ash -c "${cmd}" | tee tmp/build.log
	fi
	set -eu

	return_code=$(cat src/tmp/retcode)
	pdfmaker_retcode=$(cat src/tmp/pdfcode)
	epubmaker_retcode=$(cat src/tmp/epubcode)
	rm -f src/tmp/retcode src/tmp/pdfcode src/tmp/epubcode
	rm -f src/images/epub-cover.png
	cd tmp/ || return
	mv -f ../src/tmp/* ./
	cd ../ || return
	rm -rf src/tmp

	if [ "$return_code" -ne 0 ]; then
		echo "Build Error: ${return_code}"
		exit "$return_code"
	fi

	if [ "$pdfmaker_retcode" -eq 0 ] && [ "$epubmaker_retcode" -eq 0 ]; then
		echo "Build Success"
	elif [ "$pdfmaker_retcode" -eq 0 ] && [ "$epubmaker_retcode" -ne 0 ]; then
		echo "Warning: [EPUB]"
		exit 1
	elif [ "$pdfmaker_retcode" -ne 0 ] && [ "$epubmaker_retcode" -eq 0 ]; then
		echo "Warning: [PDF]"
		exit 1
	elif [ "$pdfmaker_retcode" -ne 0 ] && [ "$epubmaker_retcode" -ne 0 ]; then
		echo "Warning: [PDF and EPUB]"
		exit 1
	fi
}


#
# install
#
install() {
	cd src/ || return

	# .re
	if [ ! -e "articles/${1}.re" ]; then
	cat << EOF > "articles/${1}.re"
= ${1}
//lead{
前文
//}

== ほげほげ
EOF
	fi

	# catalog.yml
	if [ "$(grep -c "${1}.re" catalog.yml)" -eq 0 ]; then
		sed -i "s/^CHAPS:$/CHAPS:\n  - ${1}.re/" catalog.yml
	fi

	# src/images
	mkdir -p "images/${1}"
}



#
# Caller
#
set -eu
cd "$(dirname "$0")" || return
case "$1" in
"build") build ;;
"install") if [ $# -eq 2 ]; then install "$2"; fi ;;
esac
