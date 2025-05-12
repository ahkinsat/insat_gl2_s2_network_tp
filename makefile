.PHONY: cr2html

cr2html: cr.md styles.css
	pandoc cr.md -o cr.html --standalone --embed-resources --css styles.css 
	ln -sf ./cr.html /tmp/cr.html
