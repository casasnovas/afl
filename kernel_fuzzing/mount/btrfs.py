import argparse
import errno
import os
import subprocess
import sys

def mkfs_btrfs(path):
    subprocess.check_call(['dd', 'if=/dev/zero', 'of=' + path, 'bs=1K', 'count=16K'])
    subprocess.check_call(['mkfs.btrfs', path])

def mkdirp(path):
    try:
        os.makedirs(path)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise

def mount(path, mntpoint):
    subprocess.check_call(['mount', '-o', 'loop', path, mntpoint])

def umount(mntpoint):
    subprocess.check_call(['umount', mntpoint])

def add_some_files(mntpoint):
    subprocess.check_call(['mkdir', os.path.join(mntpoint, 'foo')])
    subprocess.check_call(['mkdir', os.path.join(mntpoint, 'foo', 'bar')])
    with open(os.path.join(mntpoint, 'foo', 'bar', 'baz'), 'w') as f:
        print >>f, 'hello world'

def analyze(path, outpath, chunksize=64):
    zeros = '\x00' * chunksize

    # Read the image in chunks of 64 bytes; we will only fuzz
    # chunks which are non-zero to start with.
    bitmap = []
    with open(path) as f:
        with open(outpath, 'w') as f2:
            while True:
                data = f.read(chunksize)
                if len(data) == 0:
                    break

                if data == zeros:
                    bitmap.append(0)
                else:
                    bitmap.append(1)
                    f2.write(data)

    print "afl test-case size: %u KiB" % ((sum(bitmap) * chunksize) / 1024)

    extents = []

    i = 0
    while True:
        try:
            j = i + bitmap[i:].index(1)
        except ValueError, e:
            break

        try:
            n = bitmap[j:].index(0)
        except ValueError, e:
            n = len(bitmap) - i

        extents.append((j * chunksize, n * chunksize))
        i = j + n

    return sum(bitmap) * chunksize, len(bitmap) * chunksize, extents

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output')

    args = parser.parse_args()

    image = 'btrfs.image'
    mntpoint = 'btrfs.mnt'

    mkfs_btrfs(image)
    mkdirp(mntpoint)
    mount(image, mntpoint)
    add_some_files(mntpoint)
    umount(mntpoint)

    # Here, .compactimage is the image with the all-0 chunks removed;
    # extents.h will contain the list of extents
    in_size, out_size, extents = analyze('btrfs.image', args.output)

    with open('btrfs-extents.h', 'w') as f:
        print >>f, "static const unsigned int in_size = %u;" % in_size
        print >>f, "static const unsigned int out_size = %u;" % out_size
        print >>f
        print >>f, "static uint8_t buffer[%u];" % in_size
        print >>f
        print >>f, "static const struct { off_t offset; size_t len; } extents[] = {"

        for offset, length in extents:
            print >>f, "\t{ %u, %u }," % (offset, length)

        print >>f, "};"

if __name__ == '__main__':
    main()
