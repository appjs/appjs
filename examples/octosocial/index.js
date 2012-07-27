var app = require('appjs'),
    github = new (require('github'))({ version: '3.0.0' }),
    KEY_F12 = process.platform === 'darwin' ? 63247 : 123;

app.serveFilesFrom(__dirname + '/assets');

var window = app.createWindow({
  width: 460,
  height: 640,
  resizable: false,
  disableSecurity: true,
  icons: __dirname + '/assets/icons'
});

window.on('create', function(){
  window.frame.show();
  window.frame.center();
});

window.on('ready', function(){
  var $ = window.$,
      $username = $('input[name=username]'),
      $password = $('input[name=password]'),
      $info = $('#info-login'),
      $label = $info.find('span'),
      $buttons = $('input, button');

  $info.error = function(label){
    this.removeClass('success').addClass('error');
    $label.text(label);
  };
  $info.succeed = function(label){
    this.removeClass('error').addClass('success');
    $label.text(label);
  };
  $.fn.disable = function(){
    this.attr('disabled','disabled');
  };
  $.fn.enable = function(){
    this.removeAttr('disabled','disabled');
  };

  $(window).on('keydown', function(e){
    if (e.keyCode === KEY_F12) {
      window.frame.openDevTools();
    }
  });

  $('#login').submit(function(e){
    e.preventDefault();

    $info.succeed('Logging in...');
    $buttons.disable();

    github.authenticate({
      type: 'basic',
      username: $username.val(),
      password: $password.val()
    });

    github.user.get({}, function(err, result) {
      if (err) {
        $info.error('Login Failed. Try Again.');
        $buttons.enable();
      } else {
        loggedIn(result);
      }
    });
  });

  function loggedIn(result){
    $info.succeed('Logged in!');
    $('#loginPage').hide();
    $('#profilePage').html(match(profileTemplate, result)).show();
    ['Followers', 'Following'].forEach(function(type){
      populate(type, { user: result.login });
    });
  }

  function populate(type, user){
    github.user['get'+type](user, function(err, result){
      if (err) {
        window.console.log(err);
      } else {
        var html = result.reduce(function(ret, item){
          return ret + match(followListTemplate, item);
        }, '');
        $('#' + type.toLowerCase() + 'List > ul').html(html).show();
      }
    });
  }
});

var profileTemplate = '\
    <div class="row">\
      <div class="top"></div>\
    </div>\
    <div class="row ribbon">\
      <div class="span2">\
        <div class="avatar"><img src="%avatar_url%" width="64" height="64"/></div>\
        <div class="name"><h1>%name%</h1></div>\
      </div>\
    </div>\
    <div class="row followers">\
      <h2>Followers (%followers%)</h2>\
      <div id="followersList" >\
        <ul class="thumbnails" style="display:none">\
        </ul>\
      </div>\
    </div>\
    <div class="row following">\
      <h2>Following (%following%)</h2>\
      <div id="followingList" >\
        <ul class="thumbnails" style="display:none">\
        </ul>\
      </div>\
    </div>\
';

var followListTemplate = '\
  <li class="span2">\
    <img src="%avatar_url%" width="64" height="64" title="%name%"/>\
  </li>\
';

function match(template, obj){
  return template.replace(/(?:%(\w+?)%)/g, function(s, m){
    return obj[m] || '';
  });
}
