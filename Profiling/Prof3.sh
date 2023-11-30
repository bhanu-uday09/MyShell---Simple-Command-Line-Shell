# Create directory with 100 files of 10MB each and subdirectories recursively
echo "Creating directory with 100 files of 10MB each and subdirectories recursively..."
time {
    mkdir -p directory3
    for ((i = 1; i <= 100; i++)); do
        dd if=/dev/zero of=directory3/file${i}.txt bs=10M count=1
    done

    for ((j = 1; j <= 50; j++)); do
        mkdir -p directory3/subdir${j}
        for ((i = 1; i <= 10; i++)); do
            dd if=/dev/zero of=directory3/subdir${j}/file${i}.txt bs=10M count=1
        done
    done
}