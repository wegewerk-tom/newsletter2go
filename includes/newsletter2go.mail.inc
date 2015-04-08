<?php

/**
 * @file
 *  Newsletter2Go implementation of MailSystemInterface.
 */

/**
 * Newsletter2Go mail backend.
 */
class Newsletter2GoMailSystem implements MailSystemInterface {
  /**
   * Concatenate and wrap the e-mail body for plain-text mails.
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

    $message['id'] = $message['id'];

    // Remove newlines and carriage returns from the subject line
    $message['subject'] = str_replace("\n", "", $message['subject']);
    $message['subject'] = str_replace("\r", "", $message['subject']);

    /**
     * Leave 'body' alone. This module always assumes it's HTML formatted, and
     * sends the 'html' parameter (instead of 'text' parameter) to Newsletter2Go.
     * Then Newsletter2Go can handle transforming HTML into plaintext.
     */
    // $message['body'] = $message['body'];

    /**
     * Leave 'headers' alone. These cannot be passed to Newsletter2Go.
     */
    // $message['headers'] = $message['headers'];

    return $message;
  }

  /**
   * Send an e-mail message, using Drupal variables and default settings.
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
 *   html    Der komplette HTML-Quellcode. Wird dieser Parameter nicht angegeben,
 *           werden reine Text- Emails (plain/text) versendet.
 *
 * Optionale Parameter:
 *
 *   id      Die id des Newsletters bei Newsletter2Go. Sie wird benötigt, falls
 *            Sie die Reporting-Funktionen von Newsletter2Go über die API nutzen
 *            möchten.
 *   debug    Mit 1 Einschalten des Debug-Modus. Ist der Debug-Modus
 *            eingeschaltet, können Sie die API von Newsletter2Go testen, ohne
 *            dass die Email tatsächlich versendet wird.
 *   ref      Eine eigene Referenz, die Sie beliebig wählen können.
 *   date     Versenden der Email zeitversetzt. Es muss ein UNIX-Timestamp
 *            übergeben werden. Liegt der Timestamp in der Vergangenheit,
 *            ist er negativ oder leer wird sofort versendet. Wird kein valider
 *            UNIX-Timestamp übergeben, wird die Email nicht entgegengenommen
 *            und Fehler-code 420 wird zurückgegeben. Der Timestamp darf
 *            maximal 1 Jahr in der Zukunft liegen
 *   reply    Sie können eine alternative Reply- Adresse angeben. Das ist die
 *                 Email-Adresse, an die eine Email geschickt wird, wenn der
 *                 Empfänger auf antworten klickt.
 *   linktracking    An- bzw. Abschalten des Linktrackings. Standardmäßig ist
 *                   diese Option eingeschaltet. Wird das Linktracking
 *                   abgeschaltet, werden Links nicht umgeschrieben und damit
 *                   können Klicks auf Links nicht registriert werden.
 *   opentracking    An- bzw. Abschalten des Öffnungs- Tracking. Standardmäßig
 *                   ist diese Option eingeschaltet. Wird das Opentracking
 *                   abgeschaltet, werden Öffnungen der Emails nicht registriert.
 **/

    // If 'Return-Path' isn't already set in php.ini, we pass it separately
    // as an additional parameter instead of in the header.
    // However, if PHP's 'safe_mode' is on, this is not allowed.
    if (isset($message['headers']['Return-Path']) && !ini_get('safe_mode')) {
      $return_path_set = strpos(ini_get('sendmail_path'), ' -f');
      if (!$return_path_set) {
        $message['Return-Path'] = $message['headers']['Return-Path'];
        unset($message['headers']['Return-Path']);
      }
    }
    $mimeheaders = array();
    foreach ($message['headers'] as $name => $value) {
      $mimeheaders[] = $name . ': ' . mime_header_encode($value);
    }
    $line_endings = variable_get('mail_line_endings', MAIL_LINE_ENDINGS);
    // Prepare mail commands.
    $mail_subject = mime_header_encode($message['subject']);
    // Note: e-mail uses CRLF for line-endings. PHP's API requires LF
    // on Unix and CRLF on Windows. Drupal automatically guesses the
    // line-ending format appropriate for your system. If you need to
    // override this, adjust $conf['mail_line_endings'] in settings.php.
    $mail_body = preg_replace('@\r?\n@', $line_endings, $message['body']);
    // For headers, PHP's API suggests that we use CRLF normally,
    // but some MTAs incorrectly replace LF with CRLF. See #234403.
    $mail_headers = join("\n", $mimeheaders);

    // We suppress warnings and notices from mail() because of issues on some
    // hosts. The return value of this method will still indicate whether mail
    // was sent successfully.
    if (!isset($_SERVER['WINDIR']) && strpos($_SERVER['SERVER_SOFTWARE'], 'Win32') === FALSE) {
      if (isset($message['Return-Path']) && !ini_get('safe_mode')) {
        // On most non-Windows systems, the "-f" option to the sendmail command
        // is used to set the Return-Path. There is no space between -f and
        // the value of the return path.
        $mail_result = @mail(
          $message['to'],
          $mail_subject,
          $mail_body,
          $mail_headers,
          '-f' . $message['Return-Path']
        );
      }
      else {
        // The optional $additional_parameters argument to mail() is not
        // allowed if safe_mode is enabled. Passing any value throws a PHP
        // warning and makes mail() return FALSE.
        $mail_result = @mail(
          $message['to'],
          $mail_subject,
          $mail_body,
          $mail_headers
        );
      }
     }
     else {
      // On Windows, PHP will use the value of sendmail_from for the
      // Return-Path header.
      $old_from = ini_get('sendmail_from');
      ini_set('sendmail_from', $message['Return-Path']);
      $mail_result = @mail(
         $message['to'],
         $mail_subject,
         $mail_body,
         $mail_headers
       );
      ini_set('sendmail_from', $old_from);
     }
     return $mail_result;
  }
}
