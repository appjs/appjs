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
			<div id="followerList" >\
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

var processProfile = function( accountDetail ) {
	var accountDetail = JSON.parse(accountDetail);
	var result = profileTemplate.replace(/%name%/,accountDetail['name']);

	    result = result.replace(/%avatar_url%/,accountDetail['avatar_url']);
	    result = result.replace(/%followers%/,accountDetail['followers']);
	    result = result.replace(/%following%/,accountDetail['following']);

	$('#profilePage').html(result).css({display:'block'});

	$.get("/followers",{user:accountDetail.login},function(data){
		data = JSON.parse(data);
		var result = "";
		data.forEach(function(follower,index){
			result += followListTemplate
						.replace(/%name%/,follower['name'])
						.replace(/%avatar_url%/,follower['avatar_url']);
		});
		$('#followerList > ul').html(result).css({display:'block'});
	}).error(function(){
		console.log('error');
	});

	$.get("/following",{user:accountDetail.login},function(data){
		data = JSON.parse(data);
		var result = "";
		data.forEach(function(follower,index){
			console.log(follower);
			result += followListTemplate
						.replace(/%name%/,follower['name'])
						.replace(/%avatar_url%/,follower['avatar_url']);
		});
		$('#followingList > ul').html(result).css({display:'block'});
	}).error(function(){
		console.log('error');
	});
};

				
			