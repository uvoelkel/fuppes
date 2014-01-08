
var running = false;

self.onmessage = function(e) {

  switch(e.data) {

    case 'start':
      running = true;
      run();
      break;
    case 'stop':
      running = false;
      break;
  }
};

self.run = function() {

  if(!running)
    return;

  var request = new XMLHttpRequest();
  request.onreadystatechange = function () {
	  if(request.readyState == 4)
	    self.postMessage(request.responseText);
  };
  request.open('GET', '/log', false);
  request.send();

  setTimeout(this.run, 1000);
}

