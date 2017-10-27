#!/usr/local/bin/ruby -w

$LOAD_PATH.unshift(File.dirname(__FILE__) + "/../lib")
require 'afl'

raise "AFL did not initialize properly" unless AFL.init

line = STDIN.read

if line[0] == 'a'
  if line[1] == 'b'
    if line[2] == 'c'
      raise "You got me!"
    end
  end
end
