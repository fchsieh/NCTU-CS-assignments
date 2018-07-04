# Web Service
+ Virtual Host (5%)
使用 ip 和 Domain 瀏覽網站根目錄會看到不同內容(5%)
+ Hide Server Token (5%)
瀏覽 php 網頁時，Response header 中不包含 php 相關資訊 (1%)
網站的 Response header 只顯示沒有版本的 Apache 資訊可以得到 (2%)，如果完全隱藏 Apache 資訊或是偽裝成其他webservice 的名字則得 (4%) (X)
+ HTTPS (30%)
啟用 HTTPS (5%) HTTP auto redirect HTTPS (5%) 啟用 HSTS (10%) 所有 https 頁面皆啟用 HTTP/2 (10%)
+ Access Control / Rewrite (10%)
僅 140.113.235.0/24 可瀏覽 http://{your apache server ip} (回傳200)、其他 IP (包含 localhost) 禁止瀏覽 (請回傳 403) (5%)
瀏覽 https://{your domain}/home/[a-zA-Z0-9]+/.* 會轉址到 http://people.cs.nctu.edu.tw/~[a-zA-Z0-9]+/.* (5%)
+ PHP (10%)
PHP 版本為 7 以上 (5%)
瀏覽 https://{your domain}/phpinfo-{your student IDnumber}.php，會顯示 php info 頁面 (5%)
# Database
+ MySQL / MariaDB (10%)
# HTTP Application
+ phpMyAdmin (5%)
存取位置必須為 https://{your domain}/phpmyadmin-{yourstudent ID number} 若非該位置 phpMyAdmin 部份不計分
140.113.235.0/24 可直接看到 phpmyadmin 登入畫面
+ Wordpress (5%)
+ Basic PHP router (10%)
於網站根目錄建立資料夾 app，裡面只能有一個檔案，檔名為 index.php，請調整 apache 設定及撰寫 index.php，來達到以下三個要求 (10%)
* 瀏覽 https://{your domain}/app/，回傳網頁內容 route /
* 瀏覽 https://{your domain}/app/{string}，回傳網頁內容 route/{string}
* 瀏覽 https://{your domain}/app/hello?name={string}，回傳網頁內容 Hello, {string}

***
## VirtualHost
**location `/usr/local/etc/apache24/extra/httpd-vhosts.conf`**
```powershell
<Directory "/usr/local/www/apache24/data"> #domain
	Options +FollowSymLinks
	RewriteEngine On
	RewriteCond %{REQUEST_FILENAME} !-f
	RewriteCond %{REQUEST_FILENAME} !-d
	RewriteCond %{REQUEST_URI} !^/home/ 
	#除了作業要求的頁面，其他都轉到/index.php
	RewriteRule ^(.*)$ /index.php [L,QSA] 
</Directory>

<Directory "/usr/local/www/apache24/data2"> #ip瀏覽 導到另一個index(和domain瀏覽不同)
	Require ip 140.113.235.0/24 #作業需求 僅140.113.235.0/24可瀏覽
</Directory>

<Directory "/usr/local/www/apache24/data/phpmyadmin-學號">
    AllowOverride AuthConfig
    Options MultiViews Indexes SymLinksIfOwnerMatch IncludesNoExec
    Order allow,deny
    Allow from 140.113.235.0/24 #140.113.235.0/24可直接進入 其他需經過basic auth登入
	Satisfy Any
</Directory>

<VirtualHost *:80>
    ServerName <your domain name>
	DocumentRoot "/usr/local/www/apache24/data"
    RedirectMatch 301 ^/(.*)$ https://<your domain name>/$1 #轉址到https
</VirtualHost>

<VirtualHost *:80>
    ServerName <server ip>
    DocumentRoot "/usr/local/www/apache24/data2"
</VirtualHost>

<VirtualHost  *:443>
    ServerName <domain name>
    DocumentRoot "/usr/local/www/apache24/data"
    SSLEngine On
    SSLCertificateFile "/usr/local/etc/apache24/certs/<domain name>/fullchain.pem" #Let's encrypt的位置
    SSLCertificateKeyFile "/usr/local/etc/apache24/certs/<domain name>/privkey.pem" 
    Header always set Strict-Transport-Security "max-age=63072000; includeSubdomains; preload"
    RewriteEngine on
    RewriteCond %{REQUEST_URI} ^/home/
    RewriteRule ^/home/(.*)$ http://people.cs.nctu.edu.tw/~$1 
</VirtualHost>
```
