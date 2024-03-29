/**
 * Unfortunately, wanting to make cross platform required static lists of hosts
 *and dns servers At least until I think of a good way to do it Hosts came from
 *alexa top 50 one day and dns are from quick searching for common servers DNS
 *only takes IPv4 server addresses The definitions must be the same as the size
 *of the arrays, as they are required later on to be read properly
 **/

#define NUM_HOSTNAMES 50
#define NUM_DNS 26

const char HOSTNAMES[NUM_HOSTNAMES][20] = {
	"google.com",     "youtube.com",
	"tmail.com",      "baidu.com",
	"facebook.com",   "qq.com",
	"sohu.com",       "taobao.com",
	"360.cn",         "jd.com",
	"yahoo.com",      "amazon.com",
	"wikipedia.org",  "zoom.us",
	"weibo.com",      "sina.com.cn",
	"live.com",       "reddit.com",
	"netflix.com",    "Xinhuanet.com",
	"Microsoft.com",  "Okezone.com",
	"Office.com",     "Vk.com",
	"Instagram.com",  "Alipay.com",
	"Csdn.net",       "Microsoftonline.com",
	"Myshopify.com",  "Yahoo.co.jp",
	"Bongacams.com",  "Twitch.tv",
	"Panda.tv",       "Zhanqi.tv",
	"Google.com.hk",  "Naver.com",
	"Bing.com",       "Amazon.in",
	"ebay.com",       "Apple.com",
	"China.com.cn",   "Tianya.cn",
	"Aliexpress.com", "Amazon.co.jp",
	"Google.co.in",   "Stackoverflow.com",
	"Livejasmin.com", "Adobe.com",
	"Tribunnews.com", "Yandex.ru",
};

const char DNS_SERVERS[NUM_DNS][20] = {
	"1.1.1.1",        "1.0.0.1",         // Cloudflare
	"8.8.8.8",        "8.8.4.4",         // Google
	"9.9.9.9",        "149.122.122.122", // Quad9
	"208.67.222.222", "208.67.220.220",  // OpenDNS home
	"185.228.168.9",  "185.220.169.9",   // CleanBrowsing
	"64.6.64.6",      "64.6.65.6",       // Verisign
	"198.101.242.72", "23.253.163.53",   // Alternate DNS
	"96.140.14.14",   "94.140.15.15",    // AdGuard DNS
	"8.26.56.26",     "8.20.247.20",     // Comodo Secure DNS
	"84.200.69.80",   "84.200.70.40",    // DNS Watch
	"77.88.8.8",      "77.88.88.1",      // Yandex
	"91.239.100.100", "89.233.43.71",    // UnsensoredDNS
	"156.154.70.1",   "156.154.71.2",    // Neustar
};
