module.exports = {
  apps : [{
    name: "endpoint",
    script: "./endpoint/bin/www.js",
    watch: true,
    watch_delay: 1000,
    ignore_watch: ["node_modules", "dbjson", "log","./node_modules", "./dbjson", "./log"],
    watch_delay: 1000,
    watch_options: {
        followSymlinks: false,
    },
    error_file: "log/pm2/endpoint-error.log",
    out_file: "log/pm2/endpoint-out.log",
    time: true
  }]
}
