for file in *.c; do
  vim -c 'Stdheader' -c wq "$file"
done
