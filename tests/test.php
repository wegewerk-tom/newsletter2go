<?php

/************************************
 * ## Fragen für Amnesty:
 *  Darf die email addressen bei Newsletter2go gespeichert werden?
 *  Darf die "gruppen" (Regionen, Themen) bei N2go gespeichert werden?
 *  Wann nicht, darf Hash-Codes für gruppen angewendet sein?
 * ## Fragen für Team wegewerk:
 *  Tauschen mit N2Go für das "auto-aufbau Newsletters" Feature?
 *  Wir können dafür:
 **  Verbessertes beispiel code auf PHP mit N2go mitteilen.
 **  Die neue Feature testen.
 * ## Fragen für Ryan (er soll mich irgendwann anrufen)
 *  Was wird von Newsletter2go aufgebaut?
 *******************************************************************/

// Credits:
// * https://github.com/sschueller/newsletter2go
// * https://www.newsletter2go.de/features/api-beispiel-implementierungen/
// * https://www.newsletter2go.de/pr/api/PHP_Example.zip?17e058
include_once('newsletter2go/newsletter2go.class.php');

$n2go = new newsletter2go('xxxyyyzzz');


print_r(createRecipientsFromFile('test_list.json'));
////print_r($n2go->createGroup(array('name' => 'likes_cats')));
////print_r($n2go->createGroup(array('name' => 'likes_monkeys')));
exit();

$nl = test_createNL();
print_r($nl);
$nlid = $nl['value'];

$groups = $n2go->getGroups();
print("1.5. https://www.newsletter2go.de/de/api/get/groups/\n");
print_r($groups['value']);

// Add all groups to this newsletter
if(!empty($groups['value'])){
  foreach($groups['value'] as $groupid => $name) {
    $params = array(
      'nlid' => $nlid,
      'groupid' => 81221,
    );
  print("2. https://www.newsletter2go.de/de/api/set/grouptonewsletter/\n");
  print_r($params);
  print_r($n2go->addGroupToNewsletter($params));
  }
}

$params = array(
  'id' => $nlid,
);
print("3. https://www.newsletter2go.de/de/api/send/newsletter/\n");
print_r($params);
print_r($n2go->sendNL($params));

function test_createNL() {
  $n2go = new Newsletter2Go('xxxyyyzzz');

  $name = 'Test newsletter: '. date(DATE_RFC822);

  $params = array(
    'name' => $name,
    'type' => 'email',
    'subject' => $name,
    'html' => '-placeholder-',
    'text' => 'text version goes here',
    'from' => 'tsc@wegewerk.com',
    'reply' => 'tsc@wegewerk.com',
    'reference' => md5($name .'saltysalt'),
  );
print("1. https://www.newsletter2go.de/de/api/create/newsletter/\n");
print_r($params);

  $params['html'] = file_get_contents('newsletter2go.html.php');
  return($n2go->createNL($params));
}

function createRecipientsFromFile($filename) {
  $n2go = new Newsletter2Go('xxxyyyzzz');
  $list = file_get_contents($filename);
  print("List size: ". formatBytes(sizeofvar($list)));
  print_r($n2go->createRecipients(array('recipients' => $list)));
}

function test_createRecipients($count = 100) {
  $n2go = new Newsletter2Go('xxxyyyzzz');
  $i = 0;
  $list = array();
  do {
    $list[] = array(
      'email' => md5($i . 'saltymcsalterson2') .'@wegewerk.com',
      'group1' => 'tmpgroup',
    );
    $i++;
  } while ( $i <= $count);

  //// print_r($list);

  $list = json_encode($list);

//  print("List size: ". formatBytes(sizeofvar($list)));

  $n2go->createRecipients(array('recipients' => $list));

  /*******
  $i = 10,000

  d08f4b409b286a3:newsletter2go-master admin$ time php test.php

  real  0m38.487s
  user  0m0.202s
  sys 0m0.050s

  $i = 20,000 // including "10,000 Duplicates"

  d08f4b409b286a3:newsletter2go-master admin$ time php test.php

  real  0m44.377s
  user  0m0.208s
  sys 0m0.078s

  $i = 100,000 // All new addresses

  d08f4b409b286a3:newsletter2go-master admin$ time php test.php
  List size: 7.44 MB (@ 1MB/s up ~7 seconds to send)
  real  4m58.371s (~4:50s to process)
  user  0m0.550s
  sys 0m0.229s
  ******/
}

// Credit: http://stackoverflow.com/a/2510459
function formatBytes($bytes, $precision = 2) {
    $units = array('B', 'KB', 'MB', 'GB', 'TB');

    $bytes = (float) max($bytes, 0);
    $pow = floor(($bytes ? log($bytes) : 0) / log(1024));
    $pow = min($pow, count($units) - 1);
    $bytes /= pow(1024, $pow);
    return round($bytes, $precision) . ' ' . $units[$pow];
}

// Credit: http://stackoverflow.com/a/2192689
function sizeofvar($var) {
    $start_memory = memory_get_usage();
    $tmp = unserialize(serialize($var));
    return memory_get_usage() - $start_memory;
}

