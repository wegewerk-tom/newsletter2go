This module makes it easier to send emails with Newsletter2Go.

__DEPENDANCIES__

* Mail System (mailsystem)
* Libraries API (libraries)

Note: For now this module always assumes body is formatted HTML, and sends the
     'html' parameter (instead of 'text' parameter). This way
     Newsletter2Go can handle transforming the HTML into plaintext.

__INSTALLATION__

Download the Newsletter2Go PHP class into sites/all/libraries/newsletter2go.

Set the Newsletter2Go API key in settings.php or with variable_set().

    $conf['newsletter2go_key'] = 'wwwwxxxxyyyyzzzzaaabbbbccccdddd';

Other available variables:
    API language namespaces the REST requests. Default is German, 'de'.
      $conf['newsletter2go_api_lang'] = 'de';

    Debug prevents Newsletter2Go from sending and turns logging to tmp directory
    on and off. Defaults to FALSE.
      $conf['newsletter2go_debug'] = FALSE;

If you want Newsletter2GoMailSystem as the site-wide default for sending mail
there are two options.

    1) Change mailsystem config at admin/config/system/mailsystem
    2) mailsystem_set(array(mailsystem_default_id() => 'Newsletter2GoMailSystem'));

For a NON-PRODUCTION working example see the code in newsletter2go_example.

Excerpt from newsletter2go_example:
  // Set optional Newsletter2Go params.
  $params = array(
    // newsletter_id can be used for tracking in reports.
    'newsletter_id' => NULL,

    // Enables debug mode
    // No emails sent, but API requests logged to sys_get_temp_dir().
    'debug'         => 0,

    // Send date is a UNIX timestamp.
    // Negative or empty will send the email right away.
    // Otherwise will be queued for sending up to a year.
    'send_date'     => -1,

    // If ref is left unset a unique hash will be generated.
    'ref'           => "test_newsletter2go_message",

    // Reply-to can be set here.
    'reply'         => $email_address,

    // En/dis-ables link tracking
    'linktracking'  => 0,

    // En/dis-ables open tracking
    'opentracking'  => 0,
  );
