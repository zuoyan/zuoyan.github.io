var optimization = new function() {
  var self = this;

  var module_node = null;
  var log_node = null;

  this.func = null;
  this.grad = null;
  this.start = null;
  this.solver = null;
  this.data_url = null;
  this.deferred = null;
  this.on_optimized = null;

  function PostJson(obj) {
    if (typeof obj != "string") {
      obj = JSON.stringify(obj);
    }
    module_node.get(0).postMessage(obj);
  }

  function SimpleLog(msg) {
   log_node.append("<pre>" + msg + "</pre>");
   log_node.children().last().get(0).scrollIntoView();
  }

  function ErrorLog(msg) {
    log_node.append("<pre class=\"alert alert-danger\">" + msg + "</pre>");
    log_node.children().last().get(0).scrollIntoView();
  }

  function Log(o) {
    if (o.path == "trace") {
      if (o.from == "value") {
        SimpleLog("At: " + JSON.stringify(o.point) + " Value: " + JSON.stringify(o.result));
        return;
      }
      if (o.from == "gradient") {
        SimpleLog("At: " + JSON.stringify(o.point) + " Gradient: " + JSON.stringify(o.result));
        return;
      }
      if (o.from == "log") {
        SimpleLog(o.message);
        return;
      }
      SimpleLog(JSON.stringify(o));
      return;
    }
    if (o.path == "optimized") {
      SimpleLog("Minimize " + o.value + " at " + JSON.stringify(o.point));
      self.value = o.value;
      self.point = o.point;
      if (self.on_optimized) {
        var func = self.on_optimized;
        self.on_optimized = null;
        func();
      }
      return;
    }
    SimpleLog(JSON.stringify(o));
  }

  function OnOptimizationMessage(message) {
    var o = JSON.parse(message.data);
    if (o.path == "value") {
      PostJson({"path": "value", "value": self.func(o.point)});
      return ;
    }
    if (o.path == "gradient") {
      PostJson({"path": "gradient",
                "gradient": self.grad(o.point)});
      return ;
    }
    Log(o);
  }

  function PostOptimize() {
    if ($.isFunction(optimization.func) && $.isFunction(optimization.grad)) {
      SimpleLog("Optimize ...");
      var message = {"path": "optimize",
                     "solver": self.solver,
                     "point": self.start};
      PostJson(message);
    } else {
      ErrorLog("optimization.func or optimization.grad are still undefined ...");
    }
  }

  function LoadScript(url, callback) {
    SimpleLog("Loading " + url + " ...");
    return $.get(url, null, function() {
      SimpleLog("Loaded " + url);
      if (callback) {
        callback();
      }
    }, "text script").fail(function() {
      ErrorLog("Load " + url + " failed");
    });
  }

  function Optimize() {
    if (self.data_url) {
      var url = self.data_url;
      self.data_url = null;
      LoadScript(url, Optimize);
      return;
    }
    var code = $("#optimization_code").val();
    try {
      eval(code);
    } catch(error) {
      ErrorLog("Evaluate code block got error:" + error);
      return;
    }
    if (self.deferred) {
      var deferred = self.deferred;
      self.deferred = null;
      deferred.then(Optimize);
      return;
    }
    PostOptimize();
  }

  function Init() {
    document.getElementById("optimization_nacl").addEventListener('message', OnOptimizationMessage, true);
  }

  this.UIMinimize = function() {
    log_node.html("");
    var lines = $("#optimization_urls").val().split(/\n/);
    var deferred = null;
    for (var index = 0; index < lines.length; index += 1) {
      var url = lines[index];
      if (!url.length) {
        continue;
      }
      if (deferred) {
        deferred = deferred.then(function() {
          return LoadScript(url);
        });
      } else {
        deferred = LoadScript(url);
      }
    }
    if (deferred) {
      deferred.then(Optimize);
    } else {
      Optimize();
    }
  };

  this.OnReady = function () {
    log_node = $("#optimization_log");
    module_node = $("#optimization_nacl");
    module_node.ready(Init);
  };

  this.SimpleLog = SimpleLog;
};

$(optimization.OnReady);
