require "bundler/gem_tasks"
require "rake/extensiontask"
require "rake/testtask"

Rake::TestTask.new(:test) do |t|
  t.libs << "test"
  t.libs << "lib"
  t.test_files = FileList["test/**/*_test.rb"]
end

Rake::ExtensionTask.new("afl") do |ext|
  ext.lib_dir = "lib/afl"
end

task :default => :test
