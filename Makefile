CS=engine.c nn.c utils.c
CFLAGS=-Wall -ggdb
CLINK=-lm
CC=gcc

mnist_images:
	mkdir mnist_images
	curl -o mnist_images/train_images.gz http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz
	curl -o mnist_images/train_labels.gz http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz
	curl -o mnist_images/test_images.gz http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz
	curl -o mnist_images/test_labels.gz http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz
	gzip -d mnist_images/*.gz

main: main.c $(CS)
	$(CC) $(CFLAGS) -o grad main.c $(CS) $(CLINK)
