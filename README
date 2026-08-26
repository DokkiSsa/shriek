# Shriek - yell at your shell

emit events and invoke subscribed shell commands

## About

`shriek` is a simple pub/sub event bus, with minimal features of subscribing and emitting.

shell commands can subscribe to topics and later emit an event to a topic with an optional text message.

## Usage

These are the available commands:

```shell
shriek subscribe TOPIC COMMAND
shriek unsubscribe TOPIC ID
shriek update TOPIC ID COMMAND
shriek emit TOPIC [MESSAGE]
shriek list [TOPIC]
shriek validate
shriek --help
```

## FORMAT

`TOPIC` follows the pattern `\w{1,30}`. if it is empty, or does not match any available topics, it will be a no-op, except in `list` command.

`COMMAND` is an arbitrary length shell command. user is responsible for ensuring correctness, quoting and escaping it. cannot be empty.

`ID` is auto-generated identifier for each `COMMAND`. it is an integer in topic file. the next generated `ID` will be `LARGEST_ID_IN_TOPIC_FILE + 1`

`MESSAGE` is an arbitrary length string that is passed to subscribers. if it is an empty string, it will be ignored.

## Commands

### `subscribe`

Creates a subscription to `TOPIC` with `COMMAND` shell command. it generates a TOPIC file if it does not exist yet.

> silly variant
> `when`
> shriek when backup_done 'notify-send "Done"'    

### `unsubscribe`

removes the record identified by `ID` from `TOPIC`. A missing topic or ID is an error. Removing the last subscription deletes the topic file.

> silly variant
> `hush`
> shriek hush backup_done 12    

### `update`

updates the record command identified by `ID` from `TOPIC`. A missing topic or ID is an error. it keeps the same `ID`.

> silly variant
> `retune`
> shriek retune backup_done 12 'notify-send "Backup complete"'

### `emit`

`emit` triggers the topic to run its subscribers

`MESSAGE` is the optional text message that is sent to each of its subscribers. it is set as an environment variable `SHRIEK_MESSAGE`.

> silly variant
> `at`/`about`
> shriek at backup_done 'success'
> shriek about backup_done 'success'

### `list`

it lists the topics available. if a `TOPIC` param is given, it list the subscribers to that topic

> silly variant
> variation exist if optional TOPIC param is sent, for the fun of it
> functionally it remains the same
> `topics`/`victims`
> shriek topics
> shriek topics backup_done
> shriek victims
> shriek victims backup_done

### `validate`

checks all the topic files validity, and reports broken file.

> silly variant
> `clear-throat`/`soundcheck`/`mictesting123`
> shriek clear-throat
> shriek soundcheck
> shriek mictesting123

## Config

config and topic files are stored at `$XDG_CONFIG_HOME/shriek` or `$HOME/.config/shriek`. 

there is no config file as of now, as there isnt anything to config for now.

an invalid line in a topic will invalidate the entire topic and not run a single subscriber until the broken subscriber is fixed. could change later. for now this seems the least effort cost way to handle it

## Subscriber data

subscribers commands have these data available in their environment:

```text
SHRIEK_TOPIC             emitted topic
SHRIEK_MESSAGE           message, possibly empty
SHRIEK_SUB_ID            subscription command ID
SHRIEK_DEPTH             recursive emission depth
```

`SHRIEK_DEPTH` is used to prevent recursive subscriber calls from its own subscribers.
An unset or malformed incoming `SHRIEK_DEPTH` is counted as zero. 
emit increments it for subscribers and refuses recursion beyond depth 16. 
This protects against accidental loops, not hostile code, that actively changes it to get more depth. so its just a basic failsafe, not an absolute recursion protection mechanism. dont tell i didnt warn you incase this happens :P

any other custom env variables set while calling `shriek` are passed along to its subscribers. make sure to always re-export `SHRIEK_DEPTH` in a script or command called by `shriek`, that again calls `shriek`.










