

var FuppesLog =  new Class({

  initialize: function(options) {
    this.worker = null;
    this.scroll = new Fx.Scroll($('log-output').getParent());
  },

  start: function() {

    if(this.worker != null) {
      delete this.worker;
      this.worker = null;
    }

    this.worker = new Worker('fuppes-log-worker.js');
    this.worker.onmessage = this.onWorkerMessage.bind(this);
    this.worker.postMessage('start');
  },

  onWorkerMessage: function(event) {

		var result = "<div style=\"border: 1px solid green;\">";

		xml = loadXmlResult(event.data);
		xml.documentElement.getChildren().each(function(item, index) {

			result += "<p style=\"border: 1px solid red;\">";
			result += item.getElement('message').get('text');
			result += "</p>";
		});

		result += "</div>";

    $('log-output').innerHTML += result;
    this.scroll.toBottom();
  },

  stop: function() {
    this.worker.postMessage('stop');
  },

});

var fuppeslog = null;

window.addEvent('load', function() {

  $('log-output').getParent().setStyle('overflow-y', 'scroll');

  fuppeslog = new FuppesLog();

  $('btn-start').addEvent('click', function() {
    fuppeslog.start();
  });

  $('btn-stop').addEvent('click', function() {
    fuppeslog.stop();
  });

  $('btn-clear').addEvent('click', function() {
    $('log-output').innerHTML = "";
  });

});

