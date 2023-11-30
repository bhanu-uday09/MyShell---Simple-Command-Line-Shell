# Create directory with 1000 files of 10MB each
echo "Creating directory with 1000 files of 10MB each..."
time {
    mkdir -p directory2
    for ((i = 1; i <= 1000; i++)); do
        dd if=/dev/zero of=directory2/file${i}.txt bs=10M count=1
    done
}
