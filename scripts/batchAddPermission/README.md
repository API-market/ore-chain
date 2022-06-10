# ORE Batch Add Permission

## `batchAddPermission.ts`

```shell
npm install
```
- Create `.env` file using example.
- Create `addPermission-input.txt` using example file.
```shell
ts-node addPermission.ts
```

## `getLinkedPermissions.ts`

Script to get the vesting accounts that have been linked.

- Create `.env` file using example.
- Populate `MONGO_URI` with prod RO URI

```shell
npm install
ts-node getLinkedPermissions.ts
```