/** Call the callback once for each item in the array and await for each to finish in turn */
async function asyncForEach(array, callback) {
  for (let index = 0; index < array.length; index += 1) {
    // eslint-disable-next-line @typescript-eslint/semi
    // eslint-disable-next-line no-await-in-loop
    await callback(array[index], index, array);
  }
}

module.exports = {
  asyncForEach
}