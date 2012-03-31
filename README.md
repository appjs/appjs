# AppJS

AppJS is an SDK to develop desktop applications using node. Or at least
it is going to be!

*Attention:* Actually you should not use AppJS before we bump version to 
v0.1.0, it is under heavy development.

The goal is to facilitate native app development using web tools like
nodejs, html, css. This is going to be clean and easy to use.

## How to

The process is nothing except developing your app using nodejs and AppJS's
GUI module. Let's try it!

First install AppJS using `npm install appjs`. Then create a folder for
your app source code and run the following command in it:

`$ appjs init`

It creates a manifest file with default values. You can read more about
setting options in wiki.

Now it's your turn, Develop your application!

When ever you want to release your app, run the command below:

`$ appjs build`

And it gives you an executable file with a resources.json that contains all
javascript source codes for you. remember, we want a clean directory!

You are ready to publish your code!

## Packaging

As you see AppJS packages javascript code and node_modules directory into
a single file ( You can package nearly everything else but we don't recommend
that because you would need to use vfs module instead of fs and here problems
show up!). We think this is good for a single big reason:

All node modules contain unrelated stuff like documents, test files etc. that
you don't really need in production. So we simply get rid of them by packaging
usefull stuff. Beside we compress files to use less disk space.

( Another reason is that we like a clean directory, but not everyone agree with
this reason! )
