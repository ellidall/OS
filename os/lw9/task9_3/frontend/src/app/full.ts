export const test = () =>

// class Node {
// 	constructor(id, x, y, hashtags) {
// 		this.id = id
// 		this.x = x
// 		this.y = y
// 		this.hashtags = hashtags
// 		this.vx = 0 // скорость по оси x
// 		this.vy = 0 // скорость по оси y
// 	}
// }
//
// class Link {
// 	constructor(source, target) {
// 		this.source = source
// 		this.target = target
// 	}
// }
//
// class ForceDirectedGraph {
// 	constructor(nodes, links, width, height, k) {
// 		this.nodes = nodes
// 		this.links = links
// 		this.width = width
// 		this.height = height
// 		this.k = k // коэффициент силы
// 	}
//
// 	// Функция для расчета силы отталкивания между вершинами
// 	calculateRepulsiveForce(node1, node2) {
// 		const dx = node2.x - node1.x
// 		const dy = node2.y - node1.y
// 		const distance = Math.sqrt(dx * dx + dy * dy)
// 		const force = (this.k * this.k) / distance
// 		return {
// 			fx: dx * force,
// 			fy: dy * force,
// 		}
// 	}
//
// 	// Функция для расчета силы притяжения между связанными вершинами
// 	calculateAttractiveForce(link) {
// 		const sourceNode = this.nodes.find(node => node.id === link.source)
// 		const targetNode = this.nodes.find(node => node.id === link.target)
// 		if (!sourceNode || !targetNode) {
// 			return {fx: 0, fy: 0}
// 		}
//
// 		const commonHashtags = sourceNode.hashtags.filter(tag => targetNode.hashtags.includes(tag))
// 		const force = (this.k * commonHashtags.length * commonHashtags.length)
// 		const dx = targetNode.x - sourceNode.x
// 		const dy = targetNode.y - sourceNode.y
// 		return {
// 			fx: dx * force,
// 			fy: dy * force,
// 		}
// 	}
//
// 	// Функция для обновления положений вершин на каждом шаге
// 	tick() {
// 		// Рассчитываем силы для каждой пары вершин
// 		for (let i = 0; i < this.nodes.length; i++) {
// 			for (let j = i + 1; j < this.nodes.length; j++) {
// 				const node1 = this.nodes[i]
// 				const node2 = this.nodes[j]
// 				const {fx: fx1, fy: fy1} = this.calculateRepulsiveForce(node1, node2)
// 				const {fx: fx2, fy: fy2} = this.calculateRepulsiveForce(node2, node1)
// 				node1.vx += fx1
// 				node1.vy += fy1
// 				node2.vx += fx2
// 				node2.vy += fy2
// 			}
// 		}
//
// 		// Рассчитываем силы для каждой связи
// 		for (const link of this.links) {
// 			const {fx: fx1, fy: fy1} = this.calculateAttractiveForce(link)
// 			const sourceNode = this.nodes.find(node => node.id === link.source)
// 			const targetNode = this.nodes.find(node => node.id === link.target)
// 			if (sourceNode) {
// 				sourceNode.vx += fx1
// 				sourceNode.vy += fy1
// 			}
// 			if (targetNode) {
// 				targetNode.vx -= fx1 // направление противоположное для целевой вершины
// 				targetNode.vy -= fy1
// 			}
// 		}
//
// 		// Обновляем положения вершин
// 		for (const node of this.nodes) {
// 			node.x += node.vx
// 			node.y += node.vy
// 			// Добавьте здесь ограничения на положение вершин в пределах width и height, если нужно
// 		}
// 	}
// }
//
// // Пример использования:
// const nodes = [
// 	new Node('A', 100, 100, ['tag1', 'tag2']),
// 	new Node('B', 200, 200, ['tag2', 'tag3']),
// 	new Node('C', 300, 300, ['tag1', 'tag3']),
// ]
//
// const links = [
// 	new Link('A', 'B'),
// 	new Link('B', 'C'),
// ]
//
// const width = 500
// const height = 500
// const k = 0.01 // коэффициент силы
//
// const graph = new ForceDirectedGraph(nodes, links, width, height, k)
//
// // Моделируем шаги алгоритма
// for (let i = 0; i < 100; i++) {
// 	graph.tick()
// }
//
// // Выводим результаты
// console.log('Final positions:')
// console.log(nodes.map(node => ({id: node.id, x: node.x, y: node.y})))

	0
