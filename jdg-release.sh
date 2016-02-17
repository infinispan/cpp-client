#!/bin/bash

JDG_REMOTE=jdg
REDHAT_REMOTE=redhat
JDG_BRANCH=jdg-7.0.x
TEMP_BRANCH=__tmp
VERSION=8.3.0

# dist-git setup
DIST_GIT_REPO=infinispan-cpp-client
DIST_GIT_RHEL7_BRANCH=jb-dg-7-rhel-7
BRANCHES="$DIST_GIT_RHEL7_BRANCH"

USERNAME=`whoami`
FULLNAME=`getent passwd "$USERNAME" | cut -d ':' -f 5`
DATE=`date +"%a %b %d %Y"`


function prepare-src() {
	echo "Preparing release for milestone $1"
    git fetch -p $JDG_REMOTE
	git branch -D $TEMP_BRANCH 2> /dev/null
	git checkout -b $TEMP_BRANCH $JDG_REMOTE/$JDG_BRANCH
	sed -i "s/-SNAPSHOT/.$1/" CMakeLists.txt
	rm -fr build
	mkdir build
	pushd build 2> /dev/null
	cmake ..
	cpack --config CPackSourceConfig.cmake
	popd
}

function prepare-dist-git() {
    CHANGELOG="$DATE $FULLNAME <${USERNAME}@redhat.com> - $VERSION.$1-$RELEASE\n- $VERSION.$1"
    rhpkg clone $DIST_GIT_REPO
    pushd $DIST_GIT_REPO
    for i in $BRANCHES;
    do
        if [ -f ../build/infinispan-hotrod-cpp-$VERSION.$1-Source.zip ]; then
            git checkout $i

            # Update .spec file
            PREV_VERSION=`grep "Version:" infinispan-cpp-client.spec | sed -e s/"Version:\s*"//`
            PREV_RELEASE=`grep "Release:" infinispan-cpp-client.spec | sed -e s/"Release:\s*"// | sed -e s/%{?dist}//` 
 
            if [ $PREV_VERSION != $VERSION.$1 ]; then
                sed -i "s/Version:\s*$PREV_VERSION/Version:\t$VERSION.$1/" infinispan-cpp-client.spec
                RELEASE=1
            else
                RELEASE=$((PREV_RELEASE+1))
            fi
            sed -i "s/Release:\s*$PREV_RELEASE%{?dist}/Release:\t$RELEASE%{?dist}/" infinispan-cpp-client.spec
             
            CHANGELOG="$DATE $FULLNAME <${USERNAME}@redhat.com> - $VERSION.$1-$RELEASE\n- $VERSION.$1"
            sed -i "s/%changelog/%changelog\n\* $CHANGELOG\n/" infinispan-cpp-client.spec

            # Update sources
            cp ../build/infinispan-hotrod-cpp-$VERSION.$1-Source.zip .
            rhpkg new-sources infinispan-hotrod-cpp-$VERSION.$1-Source.zip

            # Commit and push
            git rm --cached infinispan-cpp-client.spec && git add infinispan-cpp-client.spec
            git commit -a -m "$VERSION.$1-$RELEASE" && rhpkg push && git rev-parse HEAD
        else
            echo "Cannot find ../build/infinispan-hotrod-cpp-$VERSION.$1-Source.zip"
            exit 1
        fi
    done
    popd
}

function perform-dist-git() {
    if [ ! -f $DIST_GIT_REPO ]; then
        rhpkg clone $DIST_GIT_REPO
    fi
    pushd $DIST_GIT_REPO

    for i in $BRANCHES;
    do
        git checkout $i
        if [ $1 ]; then
            rhpkg scratch-build
            popd
            exit
        else
            rhpkg build
        fi
    done
    popd
}

function perform-src() {
    RELEASE=`grep ::VERSION build/Version.cpp | cut -d\" -f2`
    echo "Performing release $RELEASE"
    git add CMakeLists.txt
    git commit -a -m "$RELEASE"
    git tag -a $RELEASE -m $RELEASE
    git checkout $JDG_BRANCH
    git branch -D $TEMP_BRANCH
    echo "Don't forget to push the $RELEASE tag to the $REDHAT_REMOTE remote"
    exit
}

function usage() {
	echo "Usage: $0 [--prepare MILESTONE|--perform]"
	exit 1
}

case "$1" in
    "--prepare" )
    	if [ "$#" -lt 2 ]; then
	    usage
	fi
        prepare-src "$2"
        prepare-dist-git "$2" "$3"
        ;;
    "--perform" )
        perform-dist-git "$2"
        perform-src
        ;;
    "--prepare-src" )
    	if [ "$#" -lt 2 ]; then
	    usage
	fi
        prepare-src "$2"
        ;;
    "--prepare-dist-git" )
    	if [ "$#" -lt 2 ]; then
	    usage
	fi
        prepare-dist-git "$2" "$3"
        ;;
    "--perform-dist-git" )
        perform-dist-git "$2"
        ;;
    "--perform-src" )
        perform-src
        ;;
    *)
        usage
        ;;
esac

