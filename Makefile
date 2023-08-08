battery: battery.c
	gcc $^ -o $@

clean:
	rm -f battery