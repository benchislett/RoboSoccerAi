data = `

`
items = data.split('\n').map(i => i.split(' ').filter(i => i.includes('.') || i.includes('[')).map(i => i.replace('[','').replace(']','').replace(',','')).map(Number))
console.log(items.find(x => x[1] == Math.max(...items.map(j => j[1]))))

