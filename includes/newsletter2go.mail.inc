<?php

/**
 * @file
 *   Newsletter2Go implementation of MailSystemInterface.
 */

/**
 * Newsletter2Go mail backend.
 */
class Newsletter2GoMailSystem implements MailSystemInterface {
  /**
   * Prepare e-mail $message.
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   *   'id': The drupal_mail() id of the message. Look at module source code or drupal_mail() for possible id values.
   *   'to': The address or addresses the message will be sent to.
   *         The formatting of this string will be validated with the PHP e-mail validation filter.
   *   'from': The address the message will be marked as being from, which is either
   *           a custom address or the site-wide default email address.
   *   'subject': Subject of the email to be sent. This must not contain any newline characters,
   *              or the email may not be sent properly.
   *   'body': An array of strings containing the message text. The message body is created by concatenating
   *           the individual array strings into a single text string using "\n\n" as a separator.
   *   'headers': Associative array containing mail headers, such as From, Sender, MIME-Version, Content-Type, etc.
   *   'params': An array of optional parameters supplied by the caller of drupal_mail() that
   *             is used to build the message before hook_mail_alter() is invoked.
   *   'language': The language object used to build the message before hook_mail_alter() is invoked.
   *   'send': Set to FALSE to abort sending this email message.
   *
   * @return
   *   The formatted $message.
   */
  public function format(array $message) {
    /**
     * Leave message id alone.
     *
     * The final e-mail id for e-mail altering is {$module}_{$key}
     * from drupal_mail().
     *
     * $module: A module name to invoke hook_mail() on.
     *           The {$module}_mail() hook will be called to complete
     *           the $message structure which will already contain common defaults.
     * $key:    A key to identify the e-mail sent.
     */
    // $message['id'] = $message['id'];

    /**
     * Leave to and from alone.
     */
    // $message['to'] = $message['to'];
    // $message['from'] = $message['from'];

    // Remove newlines and carriage returns from the subject line
    $message['subject'] = str_replace("\n", "", $message['subject']);
    $message['subject'] = str_replace("\r", "", $message['subject']);

    /**
     * Leave 'body' mostly alone. This module always assumes it's HTML formatted,
     * and sends the 'html' parameter (instead of 'text' parameter) to
     * Newsletter2Go. After that Newsletter2Go can handle transforming HTML into
     * plaintext.
     */
    // $message['body'] = $message['body'];

    /**
     * Leave 'headers' alone. These cannot be passed to Newsletter2Go.
     */
    // $message['headers'] = $message['headers'];

    /**
     * Leave 'language' alone for now.
     */
    // $message['language'] = $message['language'];

    /**
     * Leave 'send' alone.
     */
    // $message['send'] = $message['send'];

    /**
     * Below Newsletter2go optional parameters
     * will be processed from the $message['params'].
     *
     * Von Newsletter2Go API Dokumentation v 3.3 Newsletter2Go 12. März 2015:
     * 4.1 Versenden einer Email
     *
     * Optionale Parameter:
     *
     *   $message['params']['newsletter_id']
     *   id      Die id des Newsletters bei Newsletter2Go. Sie wird benötigt, falls
     *            Sie die Reporting-Funktionen von Newsletter2Go über die API nutzen
     *            möchten.
     **/
    $message['newsletter2go']['newsletter_id'] = empty($message['params']['newsletter_id']) ? FALSE : $message['params']['newsletter_id'];

    /**
     *   debug    Mit 1 Einschalten des Debug-Modus. Ist der Debug-Modus
     *            eingeschaltet, können Sie die API von Newsletter2Go testen, ohne
     *            dass die Email tatsächlich versendet wird.
     */
    $message['newsletter2go']['debug'] = empty($message['params']['debug']) ? 0 : (int) $message['params']['debug'];

    /**
     *   ref      Eine eigene Referenz, die Sie beliebig wählen können.
     */
    // Set a unique ref ID if none is given.
    if(empty($message['params']['ref'])) {
      $message['params']['ref'] = drupal_hash_base64(date(DATE_RFC3339) . $message['to'] . $message['from'] . $message['subject'] . serialize($message['body']));
    }
    $message['newsletter2go']['ref'] = empty($message['params']['ref']) ? FALSE : (string) $message['params']['ref'];

    /**
     *   $message['params']['send_date']
     *   date     Versenden der Email zeitversetzt. Es muss ein UNIX-Timestamp
     *            übergeben werden. Liegt der Timestamp in der Vergangenheit,
     *            ist er negativ oder leer wird sofort versendet. Wird kein valider
     *            UNIX-Timestamp übergeben, wird die Email nicht entgegengenommen
     *            und Fehler-code 420 wird zurückgegeben. Der Timestamp darf
     *            maximal 1 Jahr in der Zukunft liegen
     */
    $message['newsletter2go']['date'] = empty($message['params']['send_date']) ? FALSE : (int) $message['params']['send_date'];

    /**
     *   reply    Sie können eine alternative Reply- Adresse angeben. Das ist die
     *                 Email-Adresse, an die eine Email geschickt wird, wenn der
     *                 Empfänger auf antworten klickt.
     */
    $message['newsletter2go']['reply'] = empty($message['params']['reply']) ? FALSE : (string) $message['params']['reply'];

    /*
     *   linktracking    An- bzw. Abschalten des Linktrackings. Standardmäßig ist
     *                   diese Option eingeschaltet. Wird das Linktracking
     *                   abgeschaltet, werden Links nicht umgeschrieben und damit
     *                   können Klicks auf Links nicht registriert werden.
     */
    $message['newsletter2go']['linktracking'] = empty($message['params']['linktracking']) ? FALSE : (int) $message['params']['linktracking'];

    /**
     *   opentracking    An- bzw. Abschalten des Öffnungs- Tracking. Standardmäßig
     *                   ist diese Option eingeschaltet. Wird das Opentracking
     *                   abgeschaltet, werden Öffnungen der Emails nicht registriert.
     **/
    $message['newsletter2go']['opentracking'] = empty($message['params']['opentracking']) ? FALSE : (int) $message['params']['opentracking'];

    // Clean out unused params.
    foreach($message['newsletter2go'] as $key => $value) {
      if($value === FALSE) {
        unset($message['newsletter2go'][$key]);
      }
    }

    // Support pseudo_sess_email_authn
    if(module_exists('pseudo_sess_email_authn')) {
      $message['pseudo_sess_email_authn'] = empty($message['params']['pseudo_sess_email_authn']) ? FALSE : $message['params']['pseudo_sess_email_authn'];
    }

    return $message;
  }

  /**
   * Send an e-mail message, using Newsletter2Go.
   *
   * @see http://php.net/manual/function.mail.php
   * @see drupal_mail()
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   * @return
   *   TRUE if the mail was successfully accepted, otherwise FALSE.
   */
  public function mail(array $message) {

    // Initialize optional API params from $message
    if(empty($message['newsletter2go']) === FALSE) {
      $params_optional = $message['newsletter2go'];
    }

    // NULL triggers usage of module's default API key.
    $key = NULL;
    // NULL triggers usage of module's default language.
    $language = NULL;
    // Should always be set in format, but just-in-case...
    $debug = empty($params['debug']) ? 0 : $params['debug'];

    $n2g = newsletter2go_get_client($key, $language, $debug);

    /**
     * Von Newsletter2Go API Dokumentation v 3.3 Newsletter2Go 12. März 2015:
     * 4.1 Versenden einer Email
     *
     * Um über die Newsletter2Go API eine E-Mail versenden zu können, nutzen sie
     * PHP newsletter2go libs sendEmail Funktion. Bsp: $n2g->sendEmail($params)
     *
     * Notwendige Parameter:
     *
     *   to      Die Empfänger-Emailadresse
     *   from    Die Absender-Emailadresse
     *   subject Der Betreff der Email
     *   text    Der Text-Part der Email. Wird dieser Parameter nicht angegeben,
     *                wird automatisch aus dem html-Teil ein Text-Teil erzeugt.
     *                Bitte beachten Sie, dass nur einer der beiden Parameter
     *                text oder html notwendig ist.
     *   html    Der komplette HTML-Quellcode. Wird dieser Parameter nicht
     *            angegeben, werden reine Text- Emails (plain/text) versendet.
     *
     * Note: This module always assumes body is HTML formatted, and sends the
     * 'html' parameter (instead of 'text' parameter) to Newsletter2Go. Then
     * Newsletter2Go can handle transforming HTML into plaintext.
     */

    $body = is_array($message['body']) ? implode(" ", $message['body']) : $message['body'];

    $params_required = array(
      'to'      => trim($message['to']),
      'from'    => trim($message['from']),
      'subject' => trim($message['subject']),
      'html'    => $body,
    );

    $params = array_merge($params_optional, $params_required);
    $response = $n2g->sendEmail($params);
    if($response['status'] == 200) {
      return TRUE;
    }
    $args = array(
      '!status' => $response['status'],
      '!reason' => $response['reason'],
    );
    watchdog('newsletter2go', 'Failed to send email via Newsletter2Go. status: !status; reason: !reason.', $args, WATCHDOG_NOTICE);
    return FALSE;
  }
}
