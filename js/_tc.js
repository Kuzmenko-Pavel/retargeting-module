yottos_snowball_cache=typeof yottos_snowball_cache!=="undefined"?yottos_snowball_cache:{};function win2unicode(e){var d=unescape("\u0402\u0403\u201A\u0453\u201E\u2026\u2020\u2021\u20AC\u2030\u0409\u2039\u040A\u040C\u040B\u040F\u0452\u2018\u2019\u201C\u201D\u2022\u2013\u2014\u0098\u2122\u0459\u203A\u045A\u045C\u045B\u045F\u00A0\u040E\u045E\u0408\u00A4\u0490\u00A6\u00A7\u0401\u00A9\u0404\u00AB\u00AC\u00AD\u00AE\u0407\u00B0\u00B1\u0406\u0456\u0491\u00B5\u00B6\u00B7\u0451\u2116\u0454\u00BB\u0458\u0405\u0455\u0457");var c=function(f){if(f>=192&&f<=255){return String.fromCharCode(f-192+1040)}if(f>=128&&f<=191){return d.charAt(f-128)}return String.fromCharCode(f)};var b="";for(var a=0;a<e.length;a++){b=b+c(e.charCodeAt(a))}return b}function validate(b){b=win2unicode(b);var a="";for(var e=0;e<b.length;e++){var d=b.charCodeAt(e);if((d>47)&&(d<58)){a+=String.fromCharCode(d)}else{if((d>64)&&(d<91)){a+=String.fromCharCode(d)}else{if((d>96)&&(d<123)){a+=String.fromCharCode(d)}else{if((d>1039)&&(d<1104)){a+=String.fromCharCode(d)}else{a+=String.fromCharCode(32)}}}}}return a}function getContext(){function c(t){function u(x,y,v,z){this.s_size=x.length;this.s=this.toCharArray(x);this.substring_i=y;this.result=v;this.method=z}u.prototype.toCharArray=function(x){var z=x.length,y=new Array(z);for(var v=0;v<z;v++){y[v]=x.charCodeAt(v)}return y};function i(){var v;return{b:0,k:0,l:0,c:0,lb:0,s_c:function(x){v=x;this.c=0;this.l=x.length;this.lb=0;this.b=this.c;this.k=this.l},g_c:function(){var x=v;v=null;return x},i_g:function(A,y,x){if(this.c<this.l){var z=v.charCodeAt(this.c);if(z<=x&&z>=y){z-=y;if(A[z>>3]&(1<<(z&7))){this.c++;return true}}}return false},i_g_b:function(A,y,x){if(this.c>this.lb){var z=v.charCodeAt(this.c-1);if(z<=x&&z>=y){z-=y;if(A[z>>3]&(1<<(z&7))){this.c--;return true}}}return false},o_g:function(A,y,x){if(this.c<this.l){var z=v.charCodeAt(this.c);if(z>x||z<y){this.c++;return true}z-=y;if(!(A[z>>3]&(1<<(z&7)))){this.c++;return true}}return false},o_g_b:function(A,y,x){if(this.c>this.lb){var z=v.charCodeAt(this.c-1);if(z>x||z<y){this.c--;return true}z-=y;if(!(A[z>>3]&(1<<(z&7)))){this.c--;return true}}return false},e_s:function(z,y){if(this.l-this.c<z){return false}for(var x=0;x<z;x++){if(v.charCodeAt(this.c+x)!=y.charCodeAt(x)){return false}}this.c+=z;return true},e_s_b:function(z,y){if(this.c-this.lb<z){return false}for(var x=0;x<z;x++){if(v.charCodeAt(this.c-z+x)!=y.charCodeAt(x)){return false}}this.c-=z;return true},f_a:function(K,L){var B=0,A=L,G=this.c,y=this.l,H=0,F=0,C=false;while(true){var z=B+((A-B)>>1),J=0,D=H<F?H:F,I=K[z];for(var x=D;x<I.s_size;x++){if(G+D==y){J=-1;break}J=v.charCodeAt(G+D)-I.s[x];if(J){break}D++}if(J<0){A=z;F=D}else{B=z;H=D}if(A-B<=1){if(B>0||A==B||C){break}C=true}}while(true){var I=K[B];if(H>=I.s_size){this.c=G+I.s_size;if(!I.method){return I.result}var E=I.method();this.c=G+I.s_size;if(E){return I.result}}B=I.substring_i;if(B<0){return 0}}},f_a_b:function(K,L){var B=0,A=L,G=this.c,x=this.lb,H=0,F=0,C=false;while(true){var z=B+((A-B)>>1),J=0,D=H<F?H:F,I=K[z];for(var y=I.s_size-1-D;y>=0;y--){if(G-D==x){J=-1;break}J=v.charCodeAt(G-1-D)-I.s[y];if(J){break}D++}if(J<0){A=z;F=D}else{B=z;H=D}if(A-B<=1){if(B>0||A==B||C){break}C=true}}while(true){var I=K[B];if(H>=I.s_size){this.c=G-I.s_size;if(!I.method){return I.result}var E=I.method();this.c=G-I.s_size;if(E){return I.result}}B=I.substring_i;if(B<0){return 0}}},r_s:function(y,C,A){var x=A.length-(C-y),B=v.substring(0,y),z=v.substring(C);v=B+A+z;this.l+=x;if(this.c>=C){this.c+=x}else{if(this.c>y){this.c=y}}return x},s_ch:function(){if(this.b<0||this.b>this.k||this.k>this.l||this.l>v.length){throw ("faulty slice operation")}},s_f:function(x){this.s_ch();this.r_s(this.b,this.k,x)},s_d:function(){this.s_f("")},i_:function(y,A,z){var x=this.r_s(y,A,z);if(y<=this.b){this.b+=x}if(y<=this.k){this.k+=x}},s_t:function(){this.s_ch();return v.substring(this.b,this.k)},e_v_b:function(x){return this.e_s_b(x.length,x)}}}var s={RussianStemmer:function(){var V=[new u("\u0432",-1,1),new u("\u0438\u0432",0,2),new u("\u044B\u0432",0,2),new u("\u0432\u0448\u0438",-1,1),new u("\u0438\u0432\u0448\u0438",3,2),new u("\u044B\u0432\u0448\u0438",3,2),new u("\u0432\u0448\u0438\u0441\u044C",-1,1),new u("\u0438\u0432\u0448\u0438\u0441\u044C",6,2),new u("\u044B\u0432\u0448\u0438\u0441\u044C",6,2)],U=[new u("\u0435\u0435",-1,1),new u("\u0438\u0435",-1,1),new u("\u043E\u0435",-1,1),new u("\u044B\u0435",-1,1),new u("\u0438\u043C\u0438",-1,1),new u("\u044B\u043C\u0438",-1,1),new u("\u0435\u0439",-1,1),new u("\u0438\u0439",-1,1),new u("\u043E\u0439",-1,1),new u("\u044B\u0439",-1,1),new u("\u0435\u043C",-1,1),new u("\u0438\u043C",-1,1),new u("\u043E\u043C",-1,1),new u("\u044B\u043C",-1,1),new u("\u0435\u0433\u043E",-1,1),new u("\u043E\u0433\u043E",-1,1),new u("\u0435\u043C\u0443",-1,1),new u("\u043E\u043C\u0443",-1,1),new u("\u0438\u0445",-1,1),new u("\u044B\u0445",-1,1),new u("\u0435\u044E",-1,1),new u("\u043E\u044E",-1,1),new u("\u0443\u044E",-1,1),new u("\u044E\u044E",-1,1),new u("\u0430\u044F",-1,1),new u("\u044F\u044F",-1,1)],T=[new u("\u0435\u043C",-1,1),new u("\u043D\u043D",-1,1),new u("\u0432\u0448",-1,1),new u("\u0438\u0432\u0448",2,2),new u("\u044B\u0432\u0448",2,2),new u("\u0449",-1,1),new u("\u044E\u0449",5,1),new u("\u0443\u044E\u0449",6,2)],S=[new u("\u0441\u044C",-1,1),new u("\u0441\u044F",-1,1)],R=[new u("\u043B\u0430",-1,1),new u("\u0438\u043B\u0430",0,2),new u("\u044B\u043B\u0430",0,2),new u("\u043D\u0430",-1,1),new u("\u0435\u043D\u0430",3,2),new u("\u0435\u0442\u0435",-1,1),new u("\u0438\u0442\u0435",-1,2),new u("\u0439\u0442\u0435",-1,1),new u("\u0435\u0439\u0442\u0435",7,2),new u("\u0443\u0439\u0442\u0435",7,2),new u("\u043B\u0438",-1,1),new u("\u0438\u043B\u0438",10,2),new u("\u044B\u043B\u0438",10,2),new u("\u0439",-1,1),new u("\u0435\u0439",13,2),new u("\u0443\u0439",13,2),new u("\u043B",-1,1),new u("\u0438\u043B",16,2),new u("\u044B\u043B",16,2),new u("\u0435\u043C",-1,1),new u("\u0438\u043C",-1,2),new u("\u044B\u043C",-1,2),new u("\u043D",-1,1),new u("\u0435\u043D",22,2),new u("\u043B\u043E",-1,1),new u("\u0438\u043B\u043E",24,2),new u("\u044B\u043B\u043E",24,2),new u("\u043D\u043E",-1,1),new u("\u0435\u043D\u043E",27,2),new u("\u043D\u043D\u043E",27,1),new u("\u0435\u0442",-1,1),new u("\u0443\u0435\u0442",30,2),new u("\u0438\u0442",-1,2),new u("\u044B\u0442",-1,2),new u("\u044E\u0442",-1,1),new u("\u0443\u044E\u0442",34,2),new u("\u044F\u0442",-1,2),new u("\u043D\u044B",-1,1),new u("\u0435\u043D\u044B",37,2),new u("\u0442\u044C",-1,1),new u("\u0438\u0442\u044C",39,2),new u("\u044B\u0442\u044C",39,2),new u("\u0435\u0448\u044C",-1,1),new u("\u0438\u0448\u044C",-1,2),new u("\u044E",-1,2),new u("\u0443\u044E",44,2)],P=[new u("\u0430",-1,1),new u("\u0435\u0432",-1,1),new u("\u043E\u0432",-1,1),new u("\u0435",-1,1),new u("\u0438\u0435",3,1),new u("\u044C\u0435",3,1),new u("\u0438",-1,1),new u("\u0435\u0438",6,1),new u("\u0438\u0438",6,1),new u("\u0430\u043C\u0438",6,1),new u("\u044F\u043C\u0438",6,1),new u("\u0438\u044F\u043C\u0438",10,1),new u("\u0439",-1,1),new u("\u0435\u0439",12,1),new u("\u0438\u0435\u0439",13,1),new u("\u0438\u0439",12,1),new u("\u043E\u0439",12,1),new u("\u0430\u043C",-1,1),new u("\u0435\u043C",-1,1),new u("\u0438\u0435\u043C",18,1),new u("\u043E\u043C",-1,1),new u("\u044F\u043C",-1,1),new u("\u0438\u044F\u043C",21,1),new u("\u043E",-1,1),new u("\u0443",-1,1),new u("\u0430\u0445",-1,1),new u("\u044F\u0445",-1,1),new u("\u0438\u044F\u0445",26,1),new u("\u044B",-1,1),new u("\u044C",-1,1),new u("\u044E",-1,1),new u("\u0438\u044E",30,1),new u("\u044C\u044E",30,1),new u("\u044F",-1,1),new u("\u0438\u044F",33,1),new u("\u044C\u044F",33,1)],O=[new u("\u043E\u0441\u0442",-1,1),new u("\u043E\u0441\u0442\u044C",-1,1)],N=[new u("\u0435\u0439\u0448\u0435",-1,1),new u("\u043D",-1,2),new u("\u0435\u0439\u0448",-1,1),new u("\u044C",-1,3)],M=[33,65,8,232],D,K,I=new i();this.setCurrent=function(W){I.s_c(W)};this.getCurrent=function(){return I.g_c()};function F(){while(!I.i_g(M,1072,1103)){if(I.c>=I.l){return false}I.c++}return true}function E(){while(!I.o_g(M,1072,1103)){if(I.c>=I.l){return false}I.c++}return true}function v(){K=I.l;D=K;if(F()){K=I.c;if(E()){if(F()){if(E()){D=I.c}}}}}function z(){return D<=I.c}function G(W,Z){var X,Y;I.k=I.c;X=I.f_a_b(W,Z);if(X){I.b=I.c;switch(X){case 1:Y=I.l-I.c;if(!I.e_s_b(1,"\u0430")){I.c=I.l-Y;if(!I.e_s_b(1,"\u044F")){return false}}case 2:I.s_d();break}return true}return false}function B(){return G(V,9)}function H(W,Y){var X;I.k=I.c;X=I.f_a_b(W,Y);if(X){I.b=I.c;if(X==1){I.s_d()}return true}return false}function x(){return H(U,26)}function A(){var W;if(x()){G(T,8);return true}return false}function C(){return H(S,2)}function L(){return G(R,46)}function Q(){H(P,36)}function J(){var W;I.k=I.c;W=I.f_a_b(O,2);if(W){I.b=I.c;if(z()&&W==1){I.s_d()}}}function y(){var W;I.k=I.c;W=I.f_a_b(N,4);if(W){I.b=I.c;switch(W){case 1:I.s_d();I.k=I.c;if(!I.e_s_b(1,"\u043D")){break}I.b=I.c;case 2:if(!I.e_s_b(1,"\u043D")){break}case 3:I.s_d();break}}}this.stem=function(){v();I.c=I.l;if(I.c<K){return false}I.lb=K;if(!B()){I.c=I.l;if(!C()){I.c=I.l}if(!A()){I.c=I.l;if(!L()){I.c=I.l;Q()}}}I.c=I.l;I.k=I.c;if(I.e_s_b(1,"\u0438")){I.b=I.c;I.s_d()}else{I.c=I.l}J();I.c=I.l;y();return true}}};var r=t.substring(0,1).toUpperCase()+t.substring(1).toLowerCase()+"Stemmer";return new s[r]()}var d=function(r){var i=new c(r);return function(s){i.setCurrent(s);i.stem();return i.getCurrent()}};function f(i){if(yottos_snowball_cache[i]){w=yottos_snowball_cache[i]}else{w=new d("russian")(i);yottos_snowball_cache[i]=w}return w}function p(){var r="";r+=(document.title.replace(/[^a-zA-Zа-яА-Я]/g," ").replace(/\s+$/g,"").replace(/^\s+/g,"").replace(/[\n\t\r\f\s]{2,}/g," "));var s=document.getElementsByTagName("meta");if(s){for(var i=0,t=s.length;i<t;i++){if(s[i].name.toLowerCase()=="description"){r+=" ";r+=(s[i].content.replace(/[^a-zA-Zа-яА-Я]/g," ").replace(/\s+$/g,"").replace(/^\s+/g,"").replace(/[\n\t\r\f\s]{2,}/g," "))+" "}if(s[i].name.toLowerCase()=="keywords"){r+=" ";r+=(s[i].content.replace(/[^a-zA-Zа-яА-Я]/g," ").replace(/\s+$/g,"").replace(/^\s+/g,"").replace(/[\n\t\r\f\s]{2,}/g," "))+" "}}}return r}var b=p().toLowerCase().split(" ");var m={};var q={};var o=["бол","больш","будет","будут","как","пок","коментар","будт","был","быт","вдруг","вед","впроч","всегд","всег","всех","говор","главн","даж","друг","дальш","добав","есл","ест","жизн","зач","зде","иногд","кажет","кажд","как","когд","конечн","котор","куд","лучш","либ","межд","мен","долж","смысл","след","чита","люб","постара","помим","числ","соб","ждат","част","использ","велик","член","некотор","мног","может","можн","наконец","нег","нельз","нибуд","никогд","нич","один","опя","опубликова","перед","посл","пот","почт","разв","сво","себ","сегодн","сейчас","сказа","совс","так","теб","тепер","тогд","тог","тож","тольк","хорош","хот","чег","человек","пут","вполн","возможн","через","чтоб","чут","эт","позж","все","поэт","точн","этот","над","итог","недел","сведен","тем","город","гроз","зон","принят","балл","перв","вход","лент","оста","мир","образ","идет","выйт","нол","сил","наш","мнен","одн","ищ","выш","взял","откр","нов","удив","всем","важн","ожида","сам","ход","пущ","тег","выж","комментар","ваш","цен","коснут","слаб","ситуац","назов","уход","дол","основн","ряд","заверш","дополнен","влия","описа","меньш","двум","слаб","стал","новост","отраз","вопрос","выбор","сдела","смерт","последн","поворот","высок","опозор","текст","назов","основн","причин","групп","похож","with","that","this","about","above","after","again","against","because","been","before","being","below","between","both","cannot","could","does","down","than","that","important","partner","border","link","text","radius","none","document","height","color","title","normal","font","down","display","width","block","margin","yandex","item","type","left","padding","auto","inner","function","decorati","google","position","http","align","webkit","inherit","direct","hover","referrer","write","size","math","spacing","line","sizing","float","bottom","vert","charset","vertical","background","underline","visited","inline","unescape","value","style","visible","address","else","true","tail","iframe","adriver","white","space","collapse","content","list","window","absolute","script","random","transparent","repeat","scroll","container","piclayout","email","site","form","location","place","table","show","indent","visibility","clickable","last","agewarn","opts","toggler","errormsg","getcode","href","relative","overflow","clear","cursor","outline","index","full","baseline","hide","focus","catch","async","https","escape","round","target","blank","frameborder","scrolling","click","hidden","empty","cells","letter","static","layout","transform","word","right","weight","warn","active","used","context","undefined","counter","page","mail","body","domain","region","pointer","nowrap","family","first","data","online","push","metrika","callbacks","image","classname","protocol","init","icon","wrap","root","solid","facebook","options","defaults","offset","false","return","like","opera","frames","typeof","decoration"];o=(function(){var r={};var t=o.length;for(var s=0;s<t;s++){r[o[s]]=true}return r}());for(g=0;g<b.length;g++){var a=b[g].replace(/^\s*/,"").replace(/\s*$/,"");if(a.length>3){var k=f(a);if(!o[k]){m[k]=a;q[k]=q[k]||0;q[k]++}}}var l=[];for(yottos_sWord in q){if(q[yottos_sWord]>1){l.push({text:m[yottos_sWord],streem:yottos_sWord,frequency:q[yottos_sWord]})}}var n=l.sort(function(r,i){return(r.frequency>i.frequency)?-1:((r.frequency<i.frequency)?1:0)});var j=[];for(var g=0;g<n.length;g++){j.push(n[g].text)}var h=0;var e=[];for(var g=0;g<j.length;g++){h+=j[g].length;if(navigator.appName=="Microsoft Internet Explorer"){if(h<200){e.push(j[g])}}else{if(h<1000){e.push(j[g])}}}return(e.join(" "))}function yottos_getSearch(){var a="";var d=[{name:"Mail",pattern:"go.mail.ru",param:"q"},{name:"Google",pattern:"google.",param:"q"},{name:"Google",pattern:"google.",param:"as_q"},{name:"Live Search",pattern:"search.live.com",param:"q"},{name:"Bing",pattern:"bing",param:"q"},{name:"Rambler",pattern:"rambler.ru",param:"query"},{name:"Rambler",pattern:"rambler.ru",param:"words"},{name:"Yahoo!",pattern:"search.yahoo.com",param:"p"},{name:"Nigma",pattern:"nigma.ru",param:"s"},{name:"Nigma",pattern:"nigma.ru",param:"q"},{name:"MSN",pattern:"search.msn.com",param:"q"},{name:"Ask",pattern:"ask.com",param:"q"},{name:"QIP",pattern:"search.qip.ru",param:"query"},{name:"RapidAll",pattern:"rapidall.com",param:"keyword"},{name:"Яндекс.Картинки",pattern:"images.yandex",param:"text"},{name:"Яндекс.Mobile",pattern:"m.yandex",param:"query"},{name:"Яндекс",pattern:"hghltd.yandex",param:"text"},{name:"Яндекс",pattern:"yandex",param:"text"},{name:"УкрНет",pattern:"ukr.net",param:"q"},{name:"УкрНет",pattern:"ukr.net",param:"q"},{name:"meta",pattern:"meta.ua",param:"q"},{name:"findes",pattern:"findes.com.ua",param:"q"},{name:"webalta",pattern:"webalta",param:"q"},{name:"bigmir",pattern:"bigmir.net",param:"z"},{name:"i.ua",pattern:"i.ua",param:"q"},{name:"online.ua",pattern:"online.ua",param:"q"},{name:"liveinternet.ru",pattern:"liveinternet.ru",param:"q"},{name:"all.by",pattern:"all.by",param:"query"}];var f=document.createElement("a");f.href=document.referrer;for(var b=0;b<d.length;b++){if(-1<f.hostname.indexOf(d[b]["pattern"])){var c=f.search.replace(new RegExp("\\?","g"),"").split("&");for(var e=0;e<c.length;e++){if(c[e].split("=")[0]==d[b]["param"]){a=decodeURIComponent(c[e].split("=")[1])}}}}return a}var y_url=window.location;var y_ref=document.createElement("a");y_ref.href=document.referrer;yottos_remarketing_time=typeof yottos_remarketing_time=="number"?yottos_remarketing_time:365;yottos_ac=typeof yottos_ac=="string"?yottos_ac:"";if(typeof yottos_remarketing_redirect=="object"){if(yottos_remarketing_redirect[y_url.toString()]!=undefined){y_url=document.createElement("a");y_url=yottos_remarketing_redirect[y_url.toString()]}else{if(yottos_remarketing_redirect[y_ref.toString()]!=undefined){y_url=document.createElement("a");y_url=yottos_remarketing_redirect[y_ref.toString()]}}}function getOfferId(){if(window.location.host=="www.topshoptv.com.ua"){return product.identifier}else{if(window.location.host=="ek.ua"){return google_tag_params.ecomm_prodid}else{if(window.location.host=="rosetka.com.ua"){return dataLayer[3].productID}}}return}var rand=Math.floor(Math.random()*1000000);var iframe_id="yottos_remarketing"+rand;function yottos_iframe_remarketing_query(){return"?location="+encodeURIComponent(y_url)+"&time="+yottos_remarketing_time+"&ac="+yottos_ac+"&rand="+rand+"&search="+encodeURIComponent(validate(yottos_getSearch()))+"&context="+encodeURIComponent(validate(getContext()))+"&offer_id="+encodeURIComponent(validate(getOfferId()))}document.write("<iframe id='"+iframe_id+"' src='http://rg.yottos.com/track.fcgi"+yottos_iframe_remarketing_query()+"' width='0' height='0'  frameborder='0' scrolling='no'></iframe>");