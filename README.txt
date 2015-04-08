Set the Newsletter2Go API key in settings.php

    $conf['newsletter2go_key'] = 'wwwwxxxxyyyyzzzzaaabbbbccccdddd';

Also available
    API language namespaces the REST requests. Default is German, 'de'.
    $conf['newsletter2go_api_lang'] = 'de';

    Debug turns logging to tmp directory on and off. Defaults to FALSE.
    $conf['newsletter2go_debug'] = FALSE;

To make Newsletter2GoMailSystem the site-wide default for sending mail:

    mailsystem_set(array(mailsystem_default_id() => 'Newsletter2GoMailSystem'));
