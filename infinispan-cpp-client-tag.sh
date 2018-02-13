if [ "$#" == "1" ]; then
  IFS=. read MAJ MIN MIC QUAL EXTRA <<< $(echo $1)
  echo Tagging for MAJOR=$MAJ MINOR=$MIN MICRO=$MIC QUAL=$QUAL
  if [ -n "$QUAL" ]; then
    PAT=$MIC.$QUAL
  else
    PAT=$MIC
  fi
  echo CPACK_PACKAGE_VERSION_MAJOR=$MAJ
  echo CPACK_PACKAGE_VERSION_MINOR=$MIN
  echo CPACK_PACKAGE_VERSION_PATCH=$PAT
  if [ -n "$MAJ" ] && [ -n "$MIN" ] && [ -n "$PAT" ] && [ -z "$EXTRA" ]; then
    git checkout -b __tmp origin/master
    sed -i -e 's/set (CPACK_PACKAGE_VERSION_MAJOR *".*")/set (CPACK_PACKAGE_VERSION_MAJOR "'"$MAJ"'")/' \
    -e 's/set (CPACK_PACKAGE_VERSION_MINOR *".*")/set (CPACK_PACKAGE_VERSION_MINOR "'"$MIN"'")/' \
    -e 's/set (CPACK_PACKAGE_VERSION_PATCH *".*")/set (CPACK_PACKAGE_VERSION_PATCH "'"$PAT"'")/' CMakeLists.txt
    git add CMakeLists.txt
    sed -i "s/cppTag.*/cppTag = '$1'/" Jenkinsfile
    git add Jenkinsfile
    git commit -m  "$MAJ.$MIN.$PAT"
    git tag -a "$MAJ.$MIN.$PAT" -m "$MAJ.$MIN.$PAT"
    git push origin "$MAJ.$MIN.$PAT"
    git checkout master
    git branch -D __tmp
    exit 0
  fi
fi

echo "Usage: call me with #1 argument MAJ.MIN.MICRO[.QUALIF]"
echo "eg: $0 1.2.3-Fix"
echo -e "\t will create a tag '1.2.3.Fix' on both local and origin repo"
echo -e "\t CMakeFiles.txt will be updated according."
