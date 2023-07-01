module.exports = {
  apps : [{
    name   : "endpoint",
    script : "./bin/www.js",
    watch: true,
    watch_delay: 1000,
    ignore_watch: ["node_modules", "dbjson/*.json"],
    log_date_format: “YYYY-MM-DD HH:mm Z”,
    error_file: "log/pm2/<app name>-error-<pid>.log",
    out_file: "log/pm2/<app name>-out-<pid>.log",
    time: true,
    instances: 2,
    exec_mode: "cluster"
  }]
}
