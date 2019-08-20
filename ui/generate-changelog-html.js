const { readFileSync, writeFileSync } = require('fs');
const marked = require('marked');

// `highlight` example uses `highlight.js`
marked.setOptions({
  renderer: new marked.Renderer(),
  highlight: function(code) {
    return require('highlight.js').highlightAuto(code).value;
  },
  pedantic: false,
  gfm: true,
  breaks: false,
  sanitize: false,
  smartLists: true,
  smartypants: false,
  xhtml: false
});

const markdownStr = readFileSync('../CHANGELOG.md', 'utf-8');
const html = marked(markdownStr);
writeFileSync('CHANGELOG.html', `<div class="changelog">${html}</div>`);
console.log('CHANGELOG.html generated');
