all:
	clang -dynamiclib -fPIC -o weesleep.dylib weesleep.c
	clang -F /System/Library/Frameworks -framework Foundation -framework IOKit -o themagic themagic.c
	
install:
	mkdir -p ~/.weechat/plugins
	cp weesleep.dylib themagic ~/.weechat/plugins/

clean:
	rm weesleep.dylib themagic
