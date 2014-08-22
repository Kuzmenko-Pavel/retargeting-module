server {
	#listen   8080;
	#listen   8282;
	#listen   8383;
    listen   88;
	server_name stct.yottos.com rg.yottos.com *.rg.yottos.com rg.srv-3.yottos.com rg.srv-2.yottos.com rg.srv-6.yottos.com rg.srv-7.yottos.com rg.srv-8.yottos.com rg.srv-9.yottos.com;
	access_log  /var/log/nginx/rg.yottos.access.log;
    error_log /var/log/nginx/rg.yottos.error.log;
    root /var/www/rg.yottos;
    charset utf-8;
    
    keepalive_timeout 1ms;

    location /adshow.fcgi { 
        send_timeout 100ms;
        fastcgi_cache off;
        fastcgi_pass unix:/run/workerd/sock;
        fastcgi_buffer_size 32k;
        fastcgi_buffers 10 32k;
        fastcgi_busy_buffers_size 32k;
        fastcgi_keep_conn on;
        include fastcgi_params; 
    }

    location /track.fcgi { 
        send_timeout 100ms;
        fastcgi_cache off;
        fastcgi_pass unix:/run/workerd-track/sock;
        fastcgi_buffer_size 32k;
        fastcgi_buffers 10 32k;
        fastcgi_busy_buffers_size 32k;
        fastcgi_keep_conn on;
        include fastcgi_params; 
    } 
    
    location /redirect {
    uwsgi_pass unix:///run/uwsgi/app/redirect/socket;
    uwsgi_param SCRIPT_NAME "";
    uwsgi_read_timeout 1800;
    uwsgi_send_timeout 300;
    include uwsgi_params;
    }

}
