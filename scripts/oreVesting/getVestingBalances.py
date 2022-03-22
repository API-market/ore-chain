from eospy.cleos import Cleos
import csv

ce = Cleos('https://ore.openrights.exchange')

detailed_csv = 'detailed_vesting.csv'
detailed_arr = [['account','claimed','locked','start','end']]

total_csv = 'totals_vesting.csv'
total_arr = [['account','total_claimed','total_locked']]

more = True
next_key = ''


while more:
    table = ce.get_table('eosio.token','eosio.token','vesting',limit=20, lower_bound=next_key)
    for row in table['rows']: 
        line = [row['account'],row['total_locked'],row['total_claimed']]
        print(line)
        total_arr.append(line)
        for v in row['vesting']:
            detail = [row['account'],v['claimed'],v['locked'],v['start_time'],v['end_time']]
            print(detail)
            detailed_arr.append(detail)
    next_key = table['next_key']
    more = table['more']

    if total_csv:
        with open(total_csv, 'w') as total_file:
            writer = csv.writer(total_file)
            writer.writerows(total_arr)
    if detailed_arr:
        with open(detailed_csv, 'w') as detailed_file:
            writer = csv.writer(detailed_file)
            writer.writerows(detailed_arr)
