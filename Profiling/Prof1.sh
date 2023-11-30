# Create directory with 100 files of 1GB each
echo "Creating directory with 100 files of 1GB each..."
time {
    mkdir -p directory1
    for ((i = 1; i <= 100; i++)); do
        dd if=/dev/zero of=directory1/file${i}.txt bs=1G count=1
    done
}
